
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QThread>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->connection_check->setEnabled(0);
    ui->add_group->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{4}"), this));
    ui->studnum_getter->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{6}"), this));
    ui->delete_button->setEnabled(0);

    ui->viewstudents->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->del_view->setEditTriggers(QTableWidget::NoEditTriggers);

    connect(this->socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(connection_error()));
    connect(this->socket, SIGNAL(connected()), this, SLOT(connection_success()));
    connect(this->socket, SIGNAL(readyRead()), this, SLOT(sockReadyRead()));
    set_search_bars(AllUsers);
    reconnect();
    //emit window_initialized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reconnect()
{


    //this->msgbox->setWindowModality(Qt::NonModal);

    this->msgbox->setText("Мы соединяемся с сервером...");
    this->msgbox->setIcon(QMessageBox::Information);
    this->msgbox->setStandardButtons(QMessageBox::NoButton);
    this->msgbox->setCursor(Qt::WaitCursor);
    this->msgbox->setWindowTitle("Подключение...");
    //this->msgbox->setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);
    //msgBox.setStyleSheet("background-color:#444;color:#FFF;outline:none;");
    this->msgbox->show();


    //QThread::sleep(4);
    //qDebug() << "гатово";
    //mb.information(this, "Соединение", "", 0, 0, 0, 0, 0);

    QHostAddress host(QHostAddress::LocalHost);
    socket->connectToHost(host, 6000);



    //mb.startTimer(1000);

}

void MainWindow::connection_error()
{
    this->msgbox->done(0);
    //QMessageBox msgBox(this);
    //msgBox.critical(this, "Ошибка!", "Не вышло подключиться к серверу. Сейчас программа закроется.");
    this->msgbox->critical(this, "Ошибка!", "Не вышло подключиться к серверу. Сейчас программа закроется.");
    exit(1);
}

void MainWindow::connection_success()
{
    ui->connection_check->setText("Подключено");
    ui->connection_check->setChecked(1);
    this->msgbox->done(0);
    this->msgbox->information(this, "Подключено", "Успешное подключение к серверу!");


}

void MainWindow::set_search_bars(SearchMethod s)
{
    switch (s){
        case AllUsers:
            {
                ui->get1->setVisible(0);
                ui->get2->setVisible(0);
                ui->get3->setVisible(0);
                ui->get1->setText("");
                if (ui->get1->validator()){
                    delete ui->get1->validator();
                }
                ui->get1->setValidator(nullptr);
            }
            break;
        case ByLastName:
            {
                ui->get1->setVisible(1);
                ui->get2->setVisible(0);
                ui->get3->setVisible(0);
                ui->get1->setText("");
                ui->get1->setPlaceholderText("Фамилия");
                if (ui->get1->validator()){
                    delete ui->get1->validator();
                }
                ui->get1->setValidator(nullptr);
            }
            break;
        case ByFio:
            {
                ui->get1->setVisible(1);
                ui->get2->setVisible(1);
                ui->get3->setVisible(1);
                ui->get1->setText("");
                ui->get2->setText("");
                ui->get3->setText("");
                ui->get1->setPlaceholderText("Фамилия");
                ui->get2->setPlaceholderText("Имя");
                ui->get3->setPlaceholderText("Отчество");
                if (ui->get1->validator()){
                    delete ui->get1->validator();
                }
                ui->get1->setValidator(nullptr);
            }
            break;
        case ByGroup:
            {
                ui->get1->setVisible(1);
                ui->get2->setVisible(0);
                ui->get3->setVisible(0);
                ui->get1->setText("");
                ui->get1->setPlaceholderText("Номер Группы");
                if (ui->get1->validator()){
                    delete ui->get1->validator();
                }
                ui->get1->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{4}"), this));
            }
            break;
        case ByStudID:
            {
                ui->get1->setVisible(1);
                ui->get2->setVisible(0);
                ui->get3->setVisible(0);
                ui->get1->setText("");
                ui->get1->setPlaceholderText("Номер студенческого");
                if (ui->get1->validator()){
                    delete ui->get1->validator();
                }
                ui->get1->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{6}"), this));
            }
            break;
        }
}

void MainWindow::process_answer(QByteArray &jsonData)
{
    if (ui->tabWidget->currentIndex()==0){
        QJsonDocument json = QJsonDocument::fromJson(jsonData);
        QJsonArray jsonArray = json.array();
        clear_search_table();
        foreach (const QJsonValue &student, jsonArray) {
            ui->viewstudents->insertRow(ui->viewstudents->rowCount());
            QString last_name = student.toObject().value("last_name").toVariant().toString();
            QString first_name = student.toObject().value("first_name").toVariant().toString();
            QString middle_name = student.toObject().value("middle_name").toVariant().toString();
            QString gender = student.toObject().value("gender").toVariant().toString();
            QString group_id = student.toObject().value("group_id").toVariant().toString();
            QString student_num = student.toObject().value("student_num").toVariant().toString();


            ui->viewstudents->setItem(ui->viewstudents->rowCount()-1, 0, new QTableWidgetItem(last_name));
            ui->viewstudents->setItem(ui->viewstudents->rowCount()-1, 1, new QTableWidgetItem(first_name));
            ui->viewstudents->setItem(ui->viewstudents->rowCount()-1, 2, new QTableWidgetItem(middle_name));
            ui->viewstudents->setItem(ui->viewstudents->rowCount()-1, 3, new QTableWidgetItem(gender));
            ui->viewstudents->setItem(ui->viewstudents->rowCount()-1, 4, new QTableWidgetItem(group_id));
            ui->viewstudents->setItem(ui->viewstudents->rowCount()-1, 5, new QTableWidgetItem(student_num));
        }
    }
    else{
            QJsonDocument json = QJsonDocument::fromJson(jsonData);
            QJsonArray jsonArray = json.array();
            if (jsonArray.count()==0){
                QMessageBox::critical(this, "Ошибка", "Студент с таким номером не найден");
                ui->view_button->setChecked(0);
                ui->view_button->setText("Просмотреть");
                ui->studnum_getter->setEnabled(1);
                ui->delete_button->setEnabled(0);
                return;
            }


            clear_view_table();
            foreach (const QJsonValue &student, jsonArray) {
                //add a row for a new user
                ui->del_view->insertRow(ui->del_view->rowCount());
                QString last_name = student.toObject().value("last_name").toVariant().toString();
                QString first_name = student.toObject().value("first_name").toVariant().toString();
                QString middle_name = student.toObject().value("middle_name").toVariant().toString();
                QString gender = student.toObject().value("gender").toVariant().toString();
                QString group_id = student.toObject().value("group_id").toVariant().toString();
                QString student_num = student.toObject().value("student_num").toVariant().toString();



                ui->del_view->setItem(ui->del_view->rowCount()-1, 0, new QTableWidgetItem(last_name));
                ui->del_view->setItem(ui->del_view->rowCount()-1, 1, new QTableWidgetItem(first_name));
                ui->del_view->setItem(ui->del_view->rowCount()-1, 2, new QTableWidgetItem(middle_name));
                ui->del_view->setItem(ui->del_view->rowCount()-1, 3, new QTableWidgetItem(gender));
                ui->del_view->setItem(ui->del_view->rowCount()-1, 4, new QTableWidgetItem(group_id));
                ui->del_view->setItem(ui->del_view->rowCount()-1, 5, new QTableWidgetItem(student_num));

            }
    }
}

void MainWindow::clear_view_table()
{
    ui->del_view->clearContents();
    ui->del_view->setRowCount(0);
}

void MainWindow::clear_search_table()
{
    ui->viewstudents->clearContents();
    ui->viewstudents->setRowCount(0);
}

QByteArray MainWindow::get_json_search()
{
    QJsonObject jsonObject;

    switch (ui->select_get->currentIndex()){
        case 0:
            break;
        case 1:
            jsonObject.insert("last_name", QJsonValue(ui->get1->text()));
            break;
        case 2:
            jsonObject.insert("last_name", QJsonValue(ui->get1->text()));
            jsonObject.insert("first_name", QJsonValue(ui->get2->text()));
            jsonObject.insert("middle_name", QJsonValue(ui->get3->text()));
            break;
        case 3:
            jsonObject.insert("group_id", QJsonValue(ui->get1->text()));
            break;
        case 4:
            jsonObject.insert("student_num", QJsonValue(ui->get1->text()));
            break;
    }

    QJsonDocument json(jsonObject);
    return json.toJson();
}

QByteArray MainWindow::get_json_add()
{
    QJsonObject jsonObject;
    jsonObject.insert("last_name", QJsonValue(ui->add_lastname->text()));
    jsonObject.insert("first_name", QJsonValue(ui->add_firstname->text()));
    jsonObject.insert("middle_name", QJsonValue(ui->add_middlename->text()));
    jsonObject.insert("group_id", QJsonValue(ui->add_group->text()));
    jsonObject.insert("gender", QJsonValue(ui->add_gender->currentText()));
    QJsonDocument json(jsonObject);
    return json.toJson();
}

QByteArray MainWindow::get_json_view()
{
    QJsonObject jsonObject;
    jsonObject.insert("student_num", QJsonValue(ui->studnum_getter->text()));
    QJsonDocument json(jsonObject);
    return json.toJson();
}

void MainWindow::query_add()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);

    stream << (qint16)0 << Methods::addUser << get_json_add();
    stream.device()->seek(0);
    stream << (qint16)(message.size() - sizeof(qint16));
    socket->write(message);
}

void MainWindow::query_delete(QString snum)
{
    QJsonDocument json;
    QJsonObject json_obj;
    json_obj.insert("student_num", QJsonValue(snum));
    json.setObject(json_obj);

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);

    stream << (qint16)0 << Methods::deleteUser << json.toJson();
    stream.device()->seek(0);
    stream << (qint16)(message.size() - sizeof(qint16));
    socket->write(message);
}

void MainWindow::sockReadyRead()
{
    QDataStream inputStream(socket);

    qint16 blockSize = 0;

    if (socket->bytesAvailable() < sizeof(quint16)) {
            return;
    }

    inputStream >> blockSize;

    if (socket->bytesAvailable() < blockSize) {
            return;
    }

    Methods method;
    inputStream >> method;

    QByteArray jsonData;
    inputStream >> jsonData;
    if (method!=Methods::deleteUser and method!=Methods::addUser)
        process_answer(jsonData);
}


void MainWindow::onwindow_init()
{
    reconnect();
}

void MainWindow::on_select_get_currentIndexChanged(int index)
{
    switch (index){
        case 0:
            set_search_bars(AllUsers);
            break;
        case 1:
            set_search_bars(ByLastName);
            break;
        case 2:
            set_search_bars(ByFio);
            break;
        case 3:
            set_search_bars(ByGroup);
            break;
        case 4:
            set_search_bars(ByStudID);
            break;
    }
}

void MainWindow::on_view_button_clicked()
{
    if (ui->studnum_getter->text().length()!=6){
        QMessageBox::critical(this, "Ошибка", "Некорректный номер студенческого");
        ui->view_button->setChecked(0);
        return;
    }
    ui->view_button->setChecked(ui->view_button->isChecked());




    //Вкл предпросмотр
    if (ui->view_button->isChecked()){
        ui->view_button->setText("Изменить");
        ui->studnum_getter->setEnabled(0);
        ui->delete_button->setEnabled(1);

        QByteArray message;
        QDataStream stream(&message, QIODevice::WriteOnly);

        stream << (qint16)0;
        stream << Methods::studnum << get_json_view();
        stream.device()->seek(0);
        stream << (qint16)(message.size() - sizeof(qint16));
        socket->write(message);

    }
    else{
        ui->view_button->setText("Просмотреть");
        ui->studnum_getter->setEnabled(1);
        ui->delete_button->setEnabled(0);
        clear_view_table();
    }

}


void MainWindow::on_search_button_clicked()
{
    switch (ui->select_get->currentIndex()){
    case 0:
        break;
    case 1:
        if (ui->get1->text().length()==0){
            QMessageBox::critical(this, "Ошибка", "Фамилия не может быть пустой!");
            return;
        }
        break;
    case 2:
        if (ui->get1->text().length()==0 or ui->get2->text().length()==0){
            QMessageBox::critical(this, "Ошибка", "Фамилия и имя не могут быть пустыми!");
            return;
        }
        break;
    case 3:
        if (ui->get1->text().length()==0){
            QMessageBox::critical(this, "Ошибка", "Группа не может быть пустой!");
            return;
        }
        break;
    case 4:
        if (ui->get1->text().length()==0){
            QMessageBox::critical(this, "Ошибка", "Номер студенческого не может быть пустым!");
            return;
        }
        break;
    }

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);

    stream << (qint16)0;




    switch (ui->select_get->currentIndex()){
    case 0:
        stream << Methods::selectAll << get_json_search();
        break;
    case 1:
        stream << Methods::lastname << get_json_search();
        break;
    case 2:
        stream << Methods::fullname << get_json_search();
        break;
    case 3:
        stream << Methods::group << get_json_search();
        break;
    case 4:
        stream << Methods::studnum << get_json_search();
        break;
    }
    stream.device()->seek(0);
    stream << (qint16)(message.size() - sizeof(qint16));
    qDebug()<<message;
    socket->write(message);

}


void MainWindow::on_add_button_clicked()
{
    if (
        ui->add_firstname->text().length()==0 or
        ui->add_lastname->text().length()==0 or
        ui->add_group->text().length()!=4
        ){
        QMessageBox::critical(this, "Ошибка", "Неверные данные!\n\n"
                                              "Обратите внимание, что:\n"
                                              "- Фамилия и Имя не могут быть пустыми\n"
                                              "- Номер группы должен состоять из 4 цифр");
        return;
    }
    query_add();
    QMessageBox::information(this, "Успешно", "Студент добавлен в базу данных!");

}


void MainWindow::on_delete_button_clicked()
{
    query_delete(ui->studnum_getter->text());
    QMessageBox::information(this, "Успешно", "Студент успешно удален из базы");
    ui->view_button->setText("Просмотреть");
    ui->view_button->setChecked(0);
    ui->studnum_getter->setEnabled(1);
    ui->delete_button->setEnabled(0);
    clear_view_table();
    ui->studnum_getter->setText("");

}

