
#ifndef SERVER_H
#define SERVER_H
#include <QTCPSocket>
#include <QTcpServer>
#include "game/gamecontroler.h"

class Server: public QObject
{
    Q_OBJECT


    QTcpServer* tcpServer = new QTcpServer();
    gameControler* controller;
    QTcpSocket* player1;
    QTcpSocket* player2;
    quint64 p1_time = 0;
    quint64 p2_time = 0;


    //QByteArray prepare_answer(Methods method, QByteArray data);

public:
    Server(QObject *parent);
    Server(QObject *parent, int field_size, int numbers, int turns);

    ~Server()
    {
        delete this->tcpServer;
        delete this->controller;
    }
    void set_params(int field_size, int numbers, int turns);
    bool start_listening();
    bool is_listening();
    QString get_address();
    void shutdown();
    void init();
    //void method_handler(Message msg, QTcpSocket* player);


private slots:
    void process_connection();
    void readyRead();
    void player_left();

signals:
    void waiting_for_second();
};

#endif // SERVER_H
