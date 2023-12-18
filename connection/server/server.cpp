#include "connection/server/Server.h"
//#include <QJsonDocument>
//#include <QJsonObject>
#include <QCoreApplication>
#include "connection/message.h"
#include <QNetworkInterface>

Server::Server(QObject *parent)
{
    this->setParent(parent);
    player1 = nullptr;
    player2 = nullptr;
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(process_connection()));

};

//QByteArray Server::prepare_answer(Methods method, QByteArray jsonData)
//{
//    QByteArray answer;
//    QDataStream outputStream(&answer, QIODevice::WriteOnly);
//    switch (method) {
//    case Methods::checkConnection:{


//        outputStream << (qint16)0 << Methods::checkConnection;

//        outputStream.device()->seek(0);
//        outputStream << (qint16)(answer.size() - sizeof(qint16));
//    }
//        break;
//    case Methods::addUser:{
//        QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonData);
//        QJsonObject user = jsonResponse.object();
//        QString first_name = user.value("first_name").toVariant().toString();
//        QString last_name = user.value("last_name").toVariant().toString();
//        QString middle_name = user.value("middle_name").toVariant().toString();
//        QString gender = user.value("gender").toVariant().toString();
//        QString group_id = user.value("group_id").toVariant().toString();
//        int ret = DB->add_student(last_name, first_name, middle_name, gender, group_id, DB->get_count_in_group_new(group_id));

//        if (ret == -1) {
//            outputStream << (qint16)0 << Methods::serverError;
//        } else {
//            outputStream << (qint16)0 << Methods::addUser;
//        }
//        outputStream.device()->seek(0);
//        outputStream << (qint16)(answer.size() - sizeof(qint16));
//    }
//        break;
//    case Methods::studnum:
//    {
//        QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonData);
//        QJsonObject user = jsonResponse.object();
//        QString student_num = user.value("student_num").toVariant().toString();
//        QByteArray users = DB->search_by_student_num(student_num);

//        outputStream << (qint16)0 << Methods::studnum << users;
//        outputStream.device()->seek(0);
//        outputStream << (qint16)(answer.size() - sizeof(qint16));
//    }
//        break;
//    case Methods::fullname:{
//        QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonData);
//        QJsonObject user = jsonResponse.object();
//        QString first_name = user.value("first_name").toVariant().toString();
//        QString last_name = user.value("last_name").toVariant().toString();
//        QString middle_name = user.value("middle_name").toVariant().toString();




//        QByteArray users = DB->search_by_full_name(last_name, first_name, middle_name);

//        outputStream << (qint16)0 << Methods::fullname << users;
//        outputStream.device()->seek(0);
//        outputStream << (qint16)(answer.size() - sizeof(qint16));

//    }
//        break;
//    case Methods::lastname:{
//        QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonData);
//        QJsonObject user = jsonResponse.object();
//        QString last_name = user.value("last_name").toVariant().toString();




//        QByteArray users = DB->search_by_last_name(last_name);

//        outputStream << (qint16)0 << Methods::lastname << users;
//        outputStream.device()->seek(0);
//        outputStream << (qint16)(answer.size() - sizeof(qint16));
//    }
//        break;
//    case Methods::group:{
//        QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonData);
//        QJsonObject user = jsonResponse.object();
//        QString group_id = user.value("group_id").toVariant().toString();
//        auto ret = DB->search_by_group(group_id);

//        outputStream << (qint16)0 << Methods::group << ret;
//        outputStream.device()->seek(0);
//        outputStream << (qint16)(answer.size() - sizeof(qint16));
//    }
//        break;

//    case Methods::selectAll:{

//        //select all users
//        QByteArray users = DB->get_all();
//        //the answer
//        outputStream << (qint16)0 << Methods::selectAll << users;
//        outputStream.device()->seek(0);
//        outputStream << (qint16)(answer.size() - sizeof(qint16));
//    }
//        break;
//    case Methods::deleteUser:{

//        //delete the user
//        //json will hold one object - {id:}
//        QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonData);
//        QJsonObject user = jsonResponse.object();

//        QString student_num = user.value("student_num").toVariant().toString();
//        int ret = DB->remove_student(student_num);
//        //the answer
//        if (ret == -1) {
//            outputStream << (qint16)0 << Methods::serverError;
//        } else {
//            outputStream << (qint16)0 << Methods::deleteUser;
//        }
//        outputStream.device()->seek(0);
//        outputStream << (qint16)(answer.size() - sizeof(qint16));
//    }
//        break;

//    default:
//        break;
//    }
//    return answer;


//}

//void Server::process_connection()
//{

//    QTcpSocket *socket = tcpServer->nextPendingConnection();
//    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
//}

//void Server::readyRead()
//{
//    QTcpSocket *socket = qobject_cast<QTcpSocket*>(QObject::sender());
//    QDataStream stream(socket);

//    qint16 blockSize = 0;
//    if (socket->bytesAvailable() < sizeof(quint16)) {
//        return;
//    } //проверка размеров пакета
//    stream >> blockSize;
//    if (socket->bytesAvailable() < blockSize) {
//        return;
//    }
//    //Если ничего не отдал, выходим из функции
//    Methods method;
//    stream >> method;

//    QByteArray jsonData;
//    stream >> jsonData;

//    QByteArray answer = prepare_answer(method, jsonData);
//    qDebug() << answer;
//    socket->write(answer);
//}

bool Server::start_listening()
{
    if(!tcpServer->listen(QHostAddress::LocalHost, 6000)){
        return false;
    }
    return true;
}

bool Server::is_listening()
{
    return tcpServer->isListening();
}

QString Server::get_address()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QString ip4String;

    for(int nIter=0; nIter<list.count(); nIter++)
    {
        if(!list[nIter].isLoopback())
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol ){
                ip4String = list[nIter].toString();
                qDebug() << ip4String;
            }
    }


    return ip4String+":6000";
}

void Server::shutdown()
{
    tcpServer->close();
}

void Server::process_connection()
{
    qDebug()<<"Поступает соединение";
    if (player1 == nullptr){
        player1 = tcpServer->nextPendingConnection();
        auto msg = ServerMessages::ConnectAccept();
        msg.send_to(player1);
        qDebug()<<"Есть игрок 1";
    }
    else if (player2 == nullptr){
        player2 =  tcpServer->nextPendingConnection();
        auto msg = ServerMessages::ConnectAccept();
        msg.send_to(player2);

        qDebug()<<"Есть игрок 2";
    }
    else{

        auto other = tcpServer->nextPendingConnection();
        auto msg = ServerMessages::ConnectDecline();
        msg.send_to(other);
        //other->close();
        qDebug()<<"Игроков больше не принимаем";
    }
}

void Server::readyRead()
{

}
