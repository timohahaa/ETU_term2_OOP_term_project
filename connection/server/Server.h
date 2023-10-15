
#ifndef SERVER_H
#define SERVER_H

#include "connection/methods.h"
#include <QTCPSocket>
#include <QTcpServer>


class Server: public QObject
{
    Q_OBJECT

    QTcpServer* tcpServer = new QTcpServer();
    QByteArray prepare_answer(Methods method, QByteArray data);

public:
    Server(QObject *parent =nullptr);

private slots:
    void process_connection();
    void readyRead();
};

#endif // SERVER_H
