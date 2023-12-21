#include "connection/server/Server.h"
//#include <QJsonDocument>
//#include <QJsonObject>
#include <QCoreApplication>
#include <QNetworkInterface>
#include "../message.h"


Server::Server(QObject *parent)
{
    this->setParent(parent);
    this->controller = nullptr;
    player1 = nullptr;
    player2 = nullptr;
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(process_connection()));

}

Server::Server(QObject *parent, int field_size, int numbers, int turns)
{
    this->setParent(parent);
    this->controller = nullptr;
    this->set_params(field_size, numbers, turns);
    player1 = nullptr;
    player2 = nullptr;
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(process_connection()));

}



void Server::set_params(int field_size, int numbers, int turns)
{
    this->controller = new gameControler(field_size, numbers, turns);
};

bool Server::start_listening()
{
    if(!tcpServer->listen(QHostAddress::AnyIPv4, 6000)){
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
    auto msg = ServerMessages::EventOpponentLeft();
    if (player1){
        msg.send_to(player1);
    }
    if (player2){
        msg.send_to(player2);
    }
    tcpServer->close();
}

void Server::process_connection()
{
    qDebug()<<"Поступает соединение";
    if (player1 == nullptr){
        player1 = tcpServer->nextPendingConnection();
        connect(player1, SIGNAL(disconnected()), this, SLOT(player_left()));
        connect(player1, SIGNAL(readyRead()), this, SLOT(readyRead()));

        auto msg = ServerMessages::ConnectAccept();
        msg.send_to(player1);
        qDebug()<<"Есть игрок 1";
    }
    else if (player2 == nullptr){
        player2 =  tcpServer->nextPendingConnection();
        connect(player2, SIGNAL(disconnected()), this, SLOT(player_left()));
        connect(player2, SIGNAL(readyRead()), this, SLOT(readyRead()));

        auto msg = ServerMessages::ConnectAccept();
        msg.send_to(player2);

        qDebug()<<"Сервер: Есть игрок 2. Засылаю ивент начала";

        msg = ServerMessages::EventStartingUp();
        msg.send_to(player1);
        msg.send_to(player2);

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
    auto player = qobject_cast<QTcpSocket*>(QObject::sender());
    auto msg = Message::getFromSocket(player);
    auto json = msg.get_data().object();

    if (json.contains("method")){
        qDebug() << "СЕРВЕР: ПРИШЕЛ ЗАПРОС" << msg.get_json_string();
        if (json["method"].toString() == "timestamps"){
            quint64 time = json["time"].toVariant().toULongLong();
            if (player == player1){
                p1_time = time;
                qDebug()<<"Игрок 1 время:"<<p1_time;
            }
            else if (player == player2){
                p2_time = time;
                qDebug()<<"Игрок 2 время:"<<p2_time;
            }
            if (p1_time and p2_time){
                auto msg = ServerMessages::EventGameStarted(
                    this->controller->getN_fieldSize(),
                    this->controller->getM_numberCount(),
                    this->controller->getK_numberOfTurns());
                msg.send_to(player1);
                msg.send_to(player2);
            }
        }
        if (json["method"].toString() == "set_field"){
            QVector<QVector<int>> field;
            field.resize(this->controller->getN_fieldSize());
            auto field_json = json["field"].toArray();
            auto i = 0;
            for (auto row_json: field_json) {
                for (auto num_json: row_json.toArray()) {
                    field[i].append(num_json.toInt());
                }
                i++;
            }

            SetFieldResult result;

            if (player == player1){
                result = controller->set_field(field, pl1);
            }
            else if (player == player2){
                result = controller->set_field(field, pl2);
            }

            auto msg = ServerMessages::FieldAnswer(result);
            if (player == player1){
                msg.send_to(player1);
                if (result.ok()){
                    msg = ServerMessages::EventOpponentReady();
                    msg.send_to(player2);
                }
            }
            else if (player == player2){
                msg.send_to(player2);
                if (result.ok()){
                    msg = ServerMessages::EventOpponentReady();
                    msg.send_to(player1);
                }
            }
            if (!controller->field_empty(pl1) and !controller->field_empty(pl2)){
                bool pl1_turn = p1_time<=p2_time;
                if (pl1_turn)
                    controller->setWhos_turn(pl1);
                else
                    controller->setWhos_turn(pl2);

                msg = ServerMessages::EventNextTurn(
                    controller->getScores(pl1),
                    controller->getScores(pl2),
                    controller->getTurnsMade(),
                    pl1_turn
                    );
                msg.send_to(player1);
                msg = ServerMessages::EventNextTurn(
                    controller->getScores(pl2),
                    controller->getScores(pl1),
                    controller->getTurnsMade(),
                    !pl1_turn
                    );
                msg.send_to(player2);
            }


        }
        if (json["method"].toString() == "open_cell"){
            if (
                (player==player1 and controller->getWhos_turn()==pl1)
                or
                (player==player2 and controller->getWhos_turn()==pl2)){


                //Выслали походившему результат
                auto result = controller->openCell(
                    json["i"].toInt(),
                    json["j"].toInt()
                    );
                auto msg = ServerMessages::OpenCellAnswer(
                    json["i"].toInt(),
                    json["j"].toInt(),
                    result.number
                );
                msg.send_to(player);
                //Выслали сопернику что открыли клетку
                msg = ServerMessages::EventOpponentOpenedCell(
                    json["i"].toInt(),
                    json["j"].toInt()
                );
                if (player == player1){
                    msg.send_to(player2);
                }
                else{
                    msg.send_to(player1);
                }



                //вкинули ивент хода (даже если победа для обновы счета)
                bool pl1_turn = controller->getWhos_turn()==pl1;

                msg = ServerMessages::EventNextTurn(
                    controller->getScores(pl1),
                    controller->getScores(pl2),
                    controller->getTurnsMade(),
                    pl1_turn
                    );
                msg.send_to(player1);
                msg = ServerMessages::EventNextTurn(
                    controller->getScores(pl2),
                    controller->getScores(pl1),
                    controller->getTurnsMade(),
                    !pl1_turn
                    );
                msg.send_to(player2);

                //Если победа, отослали ивент победы;
                if (result.gameEnded){

                    //Формируем состояния для игроков

                    ServerMessages::EndStates endstate1;
                    ServerMessages::EndStates endstate2;

                    if (controller->getScores(pl1)<controller->getScores(pl2)){
                        endstate1 = ServerMessages::Lose;
                        endstate2 = ServerMessages::Win;
                    }
                    else if (controller->getScores(pl1)>controller->getScores(pl2)){
                        endstate1 = ServerMessages::Win;
                        endstate2 = ServerMessages::Lose;
                    }
                    else{
                        endstate1 = ServerMessages::Draw;
                        endstate2 = ServerMessages::Draw;
                    }

                    msg = ServerMessages::EventWin(endstate1, controller->getScores(pl1),
                                                   controller->getScores(pl2));
                    msg.send_to(player1);
                    msg = ServerMessages::EventWin(endstate2, controller->getScores(pl2),
                                                   controller->getScores(pl1));
                    msg.send_to(player2);


                }

            }


        }
    }

}

void Server::player_left()
{
    auto who_left = qobject_cast<QTcpSocket*>(QObject::sender());
    if (who_left == player1){
        if (player2){
            qDebug()<<"СЕРВЕР: Игрок 1 вышел";
            auto msg = ServerMessages::EventOpponentLeft();
            msg.send_to(player2);
        }
    }
    else if (who_left == player2){
        if (player1){
            qDebug()<<"СЕРВЕР: Игрок 2 вышел";
            auto msg = ServerMessages::EventOpponentLeft();
            msg.send_to(player1);
        }
    }
}
