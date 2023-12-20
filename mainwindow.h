
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
    void client_game_started();


    //void server_ready();


private:
    quint64 start_time;
    bool self_hosting = 0;

    Ui::MainWindow *ui;

    Server *server = nullptr;
    QGameClient *client = nullptr;
};

#endif // MAINWINDOW_H
