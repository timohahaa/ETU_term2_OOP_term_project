
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <connection/server/Server.h>
#include <connection/client/qgameclient.h>

#include <QMessageBox>
#include <QDateTime>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT
    void init_game_field(int N, int M, int K);
    void update_field_hint(SetFieldResult res, int N);
    void default_field_hint(int N);
    void start_guessing();

public:
    MainWindow(QWidget *parent = nullptr);
    QMessageBox message_box_talker;
    ~MainWindow();


private slots:
    void on_exit_button_clicked();

    void on_play_button_clicked();

    void on_back_to_menu_button_clicked();

    void on_create_server_button_clicked();

    void on_join_button_clicked();

    void on_leave_button_clicked();

    void on_cancel_connection_button_clicked();

    void show_connection_status(QString status);



    void client_connecting(); //
    void client_connect_failed();//
    void client_handshaking(); //
    void client_party_full(); //
    void client_connected(); //
    void client_opponent_left();
    void client_starting_game();
    void client_game_started(int, int, int);
    void client_field_set_success(SetFieldResult);
    void client_field_set_error(SetFieldResult);
    void client_opponent_ready();
    void opened_cell(int, int, int);
    void game_over(bool player_win, int scores1, int scores2);
    void next_turn(int, int, int, bool);

    //void server_ready();


    void on_set_field_button_clicked();

private:
    quint64 start_time;
    bool self_hosting = 0;

    Ui::MainWindow *ui;

    Server *server = nullptr;
    QGameClient *client = nullptr;

};

#endif // MAINWINDOW_H
