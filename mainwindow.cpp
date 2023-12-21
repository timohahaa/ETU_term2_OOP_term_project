
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


void MainWindow::init_game_field(int N, int, int)
{
    auto *new_player_field = new QFieldWidget(ui->centralwidget, 0, N);
    auto *new_opponent_field = new QFieldWidget(ui->centralwidget, 1, N);

    ui->player_field_layout->replaceWidget(ui->player_field, new_player_field);
    delete ui->player_field;
    ui->player_field=new_player_field;

    ui->opponent_field_layout->replaceWidget(ui->opponent_field, new_opponent_field);
    delete ui->opponent_field;
    ui->opponent_field=new_opponent_field;
    ui->opponent_field->setVisible(0);
    ui->opponentfieldlabel->setVisible(0);
    connect(ui->opponent_field, SIGNAL(cell_clicked(int,int)), this, SLOT(on_opponent_field_button_clicked(int,int)));


}
void MainWindow::default_field_hint(int M){
    QString result_text = "Правила расстановки чисел:\n"
                          " - Нельзя расставлять числа в соседних клетках\n"
                          " - Вы должны расставить все числа от 1 до "+QString::number(M)+"\n"
                          " - Нельзя расставлять одинаковые числа";

    this->ui->set_field_hint_label->setText(result_text);
}

void MainWindow::start_guessing()
{
    ui->set_field_button->setVisible(0);
    ui->set_field_hint_label->setVisible(0);
    ui->scores_label->setVisible(1);
    ui->turns_left_label->setVisible(1);
    ui->opponentfieldlabel->setVisible(1);
    ui->opponent_field->setVisible(1);
}


void MainWindow::update_field_hint(SetFieldResult res, int N)
{
    QString result_text;
    QTextStream stream(&result_text);
    if (res.ok()){
        stream << "Поле успешно установлено!\n"
                  "Дождитесь, пока противник установит поле!";
    }
    else {
        stream << "Неверно расставлено поле:\n";
        if (res.neighbours){
            stream << " - Вы расставили числа в соседних клетках\n";
        }
        if (res.not_enough_digits){
            stream << " - Вы должны расставить все числа от 1 до "<<QString::number(N)<<"\n";
        }
        if (res.extra_digits){
            stream << " - Вы расставили лишние числа\n";
        }
        stream << "\n";
        if (!res.errors.empty()){
            stream << "Красным цветом выделены клетки,\nгде вы ошиблись.";
        }




    }

    this->ui->set_field_hint_label->setText(result_text);

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->start_time = QDateTime::currentMSecsSinceEpoch();
    //qDebug() << this->start_time;
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
        server = new Server(this, N, M, K);

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
            connect(client, SIGNAL(game_started(int, int, int)), this, SLOT(client_game_started(int, int, int)));
            connect(client, SIGNAL(field_set_error(SetFieldResult)), this, SLOT(client_field_set_error(SetFieldResult)));
            connect(client, SIGNAL(field_set_success(SetFieldResult)), this, SLOT(client_field_set_success(SetFieldResult)));
            connect(client, SIGNAL(opponent_ready()), this, SLOT(client_opponent_ready()));
            connect(client, SIGNAL(next_turn(int,int,int,bool)), this, SLOT(client_next_turn(int,int,int,bool)));
            connect(client, SIGNAL(game_over(ServerMessages::EndStates, int, int)), this, SLOT(client_game_over(ServerMessages::EndStates, int, int)));
            connect(client, SIGNAL(opened_cell(int,int,int)), this, SLOT(client_opened_cell(int,int,int)));
            connect(client, SIGNAL(opponent_opened_cell(int, int)), this, SLOT(client_opponent_opened_cell(int,int)));


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
    connect(client, SIGNAL(game_started(int, int, int)), this, SLOT(client_game_started(int, int, int)));
    connect(client, SIGNAL(field_set_error(SetFieldResult)), this, SLOT(client_field_set_error(SetFieldResult)));
    connect(client, SIGNAL(field_set_success(SetFieldResult)), this, SLOT(client_field_set_success(SetFieldResult)));
    connect(client, SIGNAL(opponent_ready()), this, SLOT(client_opponent_ready()));
    connect(client, SIGNAL(next_turn(int,int,int,bool)), this, SLOT(client_next_turn(int,int,int,bool)));
    connect(client, SIGNAL(game_over(ServerMessages::EndStates, int, int)), this, SLOT(client_game_over(ServerMessages::EndStates, int, int)));
    connect(client, SIGNAL(opened_cell(int,int,int)), this, SLOT(client_opened_cell(int,int,int)));
    connect(client, SIGNAL(opponent_opened_cell(int, int)), this, SLOT(client_opponent_opened_cell(int,int)));

    self_hosting = 0;
    client->connectToHost(host, port);




    // и вся логика загрузки

    //this->ui->field2 = new QFieldWidget(this, true);
    //this->ui->field2->setVisible(0);
    //this->ui->opponentfieldlabel->setVisible(0);
}

void MainWindow::on_leave_button_clicked()
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
    client->send(ClientMessages::connect_timestamps(start_time));
}

void MainWindow::client_game_started(int N, int M, int K)
{
    init_game_field(N, M, K);
    this->ui->player_field->resetColors();
    client->max_n = N;
    client->max_k = K;
    this->default_field_hint(M);
    ui->set_field_button->setVisible(1);
    ui->set_field_button->setEnabled(1);
    ui->set_field_hint_label->setVisible(1);
    ui->scores_label->setVisible(0);
    ui->turns_left_label->setVisible(0);
    ui->stackedWidget->setCurrentIndex(GameScreen);
}

void MainWindow::client_field_set_success(SetFieldResult res)
{
    update_field_hint(res, client->max_n);
    this->ui->player_field->resetColors();
    this->ui->player_field->setLockedState();
    this->ui->set_field_button->setVisible(0);

    if (ui->player_field->getState() == QFieldWidget::Locked and this->client->is_opponent_ready){
        start_guessing();
    }
    else{
        ui->current_state_label->setText("Сейчас: Ожидание расстановки противника");
    }
}

void MainWindow::client_field_set_error(SetFieldResult res)
{
    qDebug() << "Ошибка установки поля!!";
    update_field_hint(res, client->max_n);
    ui->player_field->resetColors();
    foreach(auto coord, res.errors){
        qDebug().nospace() << coord.i << coord.j;
        ui->player_field->setCellColor(coord.i, coord.j, QFieldWidget::Error);
    }
}

void MainWindow::client_opponent_ready()
{

    client->is_opponent_ready = 1;

    if (ui->player_field->getState() == QFieldWidget::Locked and this->client->is_opponent_ready){
        start_guessing();
    }
    else{
        ui->current_state_label->setText("Сейчас: Расстановка чисел (противник готов)");
    }

}

void MainWindow::client_opened_cell(int i, int j, int val)
{
    QString value = "";
    if (val!=0){
        value = QString::number(val);
    }
    ui->opponent_field->set_cell_text(i, j, value);

}

void MainWindow::client_game_over(ServerMessages::EndStates player_win, int scores1, int scores2)
{
    QString final;
    disconnect(client, SIGNAL(connect_failed()), this, SLOT(client_connect_failed()));
    disconnect(client, SIGNAL(opponent_left()), this, SLOT(client_opponent_left()));
    switch(player_win){
    case ServerMessages::Win:
        final+="Вы выиграли!\n";
        break;
    case ServerMessages::Lose:
        final+="Вы проиграли!\n";
        break;
    case ServerMessages::Draw:
        final+="Ничья!\n";
        break;
    }
    final+="Счёт: "+QString::number(scores1)+" : "+QString::number(scores2);

    show_connection_status(final);
}

void MainWindow::client_next_turn(int scores1, int scores2, int turnsmade, bool now_turn)
{
    QString turn;
    if (now_turn){
        turn = "Сейчас: Ваш ход";
    }
    else{
        turn = "Сейчас: Ход противника";
    }
    QString scores = "Счёт: "+QString::number(scores1)+" : "+QString::number(scores2);
    QString turns = "Ход: "+ QString::number(turnsmade)+"/"+QString::number(client->max_k);

    ui->turns_left_label->setText(turns);
    ui->current_state_label->setText(turn);
    ui->scores_label->setText(scores);
    if (now_turn){
        ui->opponent_field->opponent_unlock();
    }
    else {
        ui->opponent_field->opponent_lock();
    }



}

void MainWindow::client_opponent_opened_cell(int i, int j)
{
    ui->player_field->setCellColor(j,i, QFieldWidget::Error);
}

void MainWindow::on_opponent_field_button_clicked(int i, int j)
{
    auto msg = ClientMessages::open_cell(i, j);
    client->send(msg);
}

void MainWindow::on_set_field_button_clicked()
{
    auto msg = ClientMessages::set_field(ui->player_field->get_indexes());
    client->send(msg);
}

