
#ifndef QGAMECLIENT_H
#define QGAMECLIENT_H


#include <QObject>
#include <QTcpSocket>
#include "../message.h"
#include "game/gamecontroler.h"

class QGameClient : public QObject
{
    Q_OBJECT
    void process_answer(Message msg);
    void handshake(Message msg);
    void event_handler(Message msg);
    void method_handler(Message msg);


public:
    int max_n=0;
    int max_k=0;
    bool is_opponent_ready = 0;
    explicit QGameClient(QObject *parent = nullptr);

    QTcpSocket *socket = new QTcpSocket;
    void connectToHost(QString host, quint16 port);
    void disconnect();
    void send(Message msg);

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
    void opponent_left(); //
    void starting_game(); //

    void game_started(int, int, int); //

    void field_set_success(SetFieldResult);
    void field_set_error(SetFieldResult);
    void opponent_ready();

    void opened_cell(int, int, int);
    void game_over(bool, int, int);
    void next_turn(int, int, int, bool);

};

#endif // QGAMECLIENT_H
