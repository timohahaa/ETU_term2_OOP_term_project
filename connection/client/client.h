
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include "connection/methods.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow





{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void reconnect();
    void connection_error();
    void connection_success();
    void set_search_bars(SearchMethod s);

    void process_answer(QByteArray &jsonData);
    void clear_view_table();
    void clear_search_table();

    QByteArray get_json_search();
    QByteArray get_json_add();
    QByteArray get_json_view();

    void query_add();
    void query_delete(QString snum);
    void sockReadyRead();


private slots:
    void onwindow_init();
    void on_select_get_currentIndexChanged(int index);

    void on_view_button_clicked();

    void on_search_button_clicked();

    void on_add_button_clicked();

    void on_delete_button_clicked();

private:
    Ui::MainWindow *ui;

    QTcpSocket *socket = new QTcpSocket();
    QMessageBox *msgbox = new QMessageBox();

signals:
    void window_initialized();

};

#endif // MAINWINDOW_H
