
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <connection/server/Server.h>
#include <connection/client/qgameclient.h>

#include <QMessageBox>

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

    //void server_ready();

private:
    Ui::MainWindow *ui;

    Server *server = nullptr;
    QGameClient *client = nullptr;
};

#endif // MAINWINDOW_H
