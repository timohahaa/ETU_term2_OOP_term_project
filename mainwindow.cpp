
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include "ui/qfieldwidget.h"
#include "QRegularExpression"
#include "QRegularExpressionValidator"
#include "QIntValidator"

#define MIN_M_VAL 1
#define MAX_M_VAL 9
#define MIN_N_VAL 4
#define MAX_N_VAL 9
#define MIN_K_VAL 10
#define MAX_K_VAL 50


enum Screens{
    TitleScreen, ConfigureScreen, WaitScreen, GameScreen
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    auto *M_filter = new QIntValidator(MIN_M_VAL, MAX_M_VAL);
    auto *N_filter = new QIntValidator(MIN_N_VAL, MAX_N_VAL);
    auto *K_filter = new QIntValidator(MIN_K_VAL, MAX_K_VAL);

    ui->setupUi(this);
    ui->M_input->setValidator(M_filter);
    ui->N_input->setValidator(N_filter);
    ui->K_input->setValidator(K_filter);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_exit_button_clicked()
{
    QCoreApplication::quit();
}


void MainWindow::on_play_button_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(ConfigureScreen);
}


void MainWindow::on_back_to_menu_button_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(TitleScreen);
}

void MainWindow::on_create_server_button_clicked()
{
    auto N = ui->N_input->text().toInt();
    auto M = ui->M_input->text().toInt();
    auto K = ui->K_input->text().toInt();

    if (
         M>=MIN_M_VAL and M<=MAX_M_VAL and
        N>=MIN_N_VAL and N<=MAX_N_VAL and
        K>=MIN_K_VAL and K<=MAX_K_VAL){

        qDebug()<<"Start Hosting <<" << N << "<<" << M << "<<" << K;
        this->ui->stackedWidget->setCurrentIndex(WaitScreen);
        //start hosting method;

        server = new Server(this);
        ui->connectionStateLabel->setText(QString("Инициализация сервера...\n"));
        if (!server->start_listening()){
            ui->connectionStateLabel->setText(QString("Ошибка инициализации\n"
                                                      "Попробуйте заново, и убедитесь,\n"
                                                      "что порт 6000 свободен"));
        }
        else{
            client = new QGameClient(this);
            client->connectToHost("localhost",6000);
            ui->connectionStateLabel->setText(QString("Ожидание второго игрока...\n")+
                                                      "Сервер работает на "+this->server->get_address());
        }
    }
    else{
        message_box_talker.warning(this, "Ошибка",
           "Кажется, с такими значениями невозможно начать игру\n"
           " Сторона поля (N) может быть от "+QString::number(MIN_N_VAL)+" до " + QString::number(MAX_N_VAL)+ "\n"
           " Количество чисел (M) может быть от "+QString::number(MIN_M_VAL)+" до " + QString::number(MAX_M_VAL)+ "\n"
           " Количество ходов (K) может быть от "+QString::number(MIN_K_VAL)+" до " + QString::number(MAX_K_VAL)
           );
    }


}


void MainWindow::on_join_button_clicked()
{
    auto host = ui->host_input->text();
    auto port = ui->port_input->text().toULong();
    this->ui->stackedWidget->setCurrentIndex(WaitScreen);

    ui->connectionStateLabel->setText("Соединение...");

    client = new QGameClient(this);

    client->connectToHost(host, port);

    // и вся логика загрузки

    //this->ui->field2 = new QFieldWidget(this, true);
    //this->ui->field2->setVisible(0);
    //this->ui->opponentfieldlabel->setVisible(0);
}


void MainWindow::on_leave_button_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(TitleScreen);
    //и вся логика выхода
}


void MainWindow::on_cancel_connection_button_clicked()
{
    if (client){
        client->disconnect();
        delete client;
    }
    if (server and server->is_listening()){
        server->shutdown();
        delete server;
        server = nullptr;
    }

    this->ui->stackedWidget->setCurrentIndex(TitleScreen);
    //и вся логика дисконнекта или шатдауна сервера;
}





//TODO коннект клиента
// взаимодействие клиента и сервера
// игровой процесс
// покраснение поля
// таймкоды и кто начинает первым
// стилизация


