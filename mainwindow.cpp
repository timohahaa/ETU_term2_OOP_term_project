
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
    this->start_time = QDateTime::currentMSecsSinceEpoch();
    qDebug() << this->start_time;
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



        //ЛОГИКА СТАРТА СЕРВЕРА И САМОКОННЕКТА

        this->ui->stackedWidget->setCurrentIndex(WaitScreen);

        this->show_connection_status("Инициализация сервера...");
        server = new Server(this);

        if (!server->start_listening()){
            this->show_connection_status("Ошибка инициализации\n"
                                         "Попробуйте заново, и убедитесь,\n"
                                         "что порт 6000 свободен");
        }
        else{
            self_hosting = 1;
            client = new QGameClient(this);

            connect(client, SIGNAL(connecting()), this, SLOT(client_connecting()));
            connect(client, SIGNAL(connect_failed()), this, SLOT(client_connect_failed()));
            connect(client, SIGNAL(handshaking()), this, SLOT(client_handshaking()));
            connect(client, SIGNAL(party_full()), this, SLOT(client_party_full()));
            connect(client, SIGNAL(connected()), this, SLOT(client_connected()));
            connect(client, SIGNAL(opponent_left()), this, SLOT(client_opponent_left()));
            connect(client, SIGNAL(starting_game()), this, SLOT(client_starting_game()));

            client->connectToHost("localhost",6000);
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
    connect(client, SIGNAL(connecting()), this, SLOT(client_connecting()));
    connect(client, SIGNAL(connect_failed()), this, SLOT(client_connect_failed()));
    connect(client, SIGNAL(handshaking()), this, SLOT(client_handshaking()));
    connect(client, SIGNAL(party_full()), this, SLOT(client_party_full()));
    connect(client, SIGNAL(connected()), this, SLOT(client_connected()));
    connect(client, SIGNAL(opponent_left()), this, SLOT(client_opponent_left()));
    connect(client, SIGNAL(starting_game()), this, SLOT(client_starting_game()));

    self_hosting = 0;
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
    if (server and server->is_listening()){
        server->shutdown();
        delete server;
        server = nullptr;
    }

    if (client){
        client->disconnect();
        delete client;
        client = nullptr;
    }





    this->ui->stackedWidget->setCurrentIndex(TitleScreen);
    //и вся логика дисконнекта или шатдауна сервера;
}

void MainWindow::show_connection_status(QString status)
{
    ui->stackedWidget->setCurrentIndex(WaitScreen);
    ui->connectionStateLabel->setText(status);
}

void MainWindow::client_connecting()
{
    show_connection_status("Подключение к серверу...");
}

void MainWindow::client_connect_failed()
{
    show_connection_status("Ошибка подключения к серверу:\nСервер не найден.");
}

void MainWindow::client_handshaking()
{
    show_connection_status("Обмен данными...");
}

void MainWindow::client_party_full()
{
    show_connection_status("На сервере уже максимальное количество игроков.\n"
                           "Попробуйте подключиться позже");
}

void MainWindow::client_connected()
{
    if (self_hosting){
        show_connection_status("Подключено, ждём игрока 2.\nСервер работает на "+this->server->get_address());
    }
    else {
        show_connection_status("Оба игрока на месте. Игра скоро начнётся.");
    }
}

void MainWindow::client_opponent_left()
{
    show_connection_status("Оппонент отключился от игры.");
    client->disconnect();
    delete client;
    client = nullptr;
}

void MainWindow::client_starting_game()
{
    show_connection_status("Оба игрока на месте. Игра скоро начнётся.");
    // сделать хендшейк с таймстемпами.
}









//TODO коннект клиента
// взаимодействие клиента и сервера
// игровой процесс
// покраснение поля
// таймкоды и кто начинает первым
// стилизация


