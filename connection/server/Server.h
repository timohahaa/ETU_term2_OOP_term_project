
#ifndef SERVER_H
#define SERVER_H
#include <QTCPSocket>
#include <QTcpServer>

class Server: public QObject
{
    Q_OBJECT


    QTcpServer* tcpServer = new QTcpServer();
    QTcpSocket* player1;
    QTcpSocket* player2;

    //QByteArray prepare_answer(Methods method, QByteArray data);

public:
    Server(QObject *parent);
    bool start_listening();
    bool is_listening();
    QString get_address();
    void shutdown();
    void init();

private slots:
    void process_connection();
    void readyRead();
signals:
    void waiting_for_second();

};

#endif // SERVER_H
