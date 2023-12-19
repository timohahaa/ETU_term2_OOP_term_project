
#ifndef QGAMECLIENT_H
#define QGAMECLIENT_H


#include <QObject>
#include <QTcpSocket>
#include "../message.h"

class QGameClient : public QObject
{
    Q_OBJECT
    void process_answer(Message msg);
    void handshake(Message msg);
    void event_handler(Message msg);

public:
    explicit QGameClient(QObject *parent = nullptr);

    QTcpSocket *socket = new QTcpSocket;
    void connectToHost(QString host, quint16 port);
    void disconnect();

private slots:

    void connection_error();
    void connection_success();
    void sockReadyRead();

signals:
    void connecting(); //
    void connect_failed();//
    void handshaking(); //
    void party_full(); //
    void connected(); //
    void opponent_left(); // когда начнем прописывать ивенты
    void starting_game();

    void game_started();


};

#endif // QGAMECLIENT_H
