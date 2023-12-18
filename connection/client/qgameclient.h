
#ifndef QGAMECLIENT_H
#define QGAMECLIENT_H


#include <QObject>
#include <QTcpSocket>
#include "../message.h"

class QGameClient : public QObject
{
    Q_OBJECT
public:
    explicit QGameClient(QObject *parent = nullptr);

    QTcpSocket *socket = new QTcpSocket;
    void connectToHost(QString host, quint16 port);
    void disconnect();
    void process_answer(Message msg);
private slots:

    void connection_error();
    void connection_success();
    void sockReadyRead();

signals:
    void cant_connect();
    void server_disconnected();
    void connect_and_wait();
    void connect_and_start();


    void start_game();


};

#endif // QGAMECLIENT_H
