#include <QMainWindow>
#include <QJsonDocument>
#include <QDataStream>
#include <QIODevice>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <game/gamecontroler.h>
class Message{
    QByteArray data;

public:
    static Message from_json(QJsonDocument json)
    {
        Message newm;
        newm.data = json.toJson();
        return newm;
    }
    static Message from_string(QString string){
        return from_json(QJsonDocument::fromJson(string.toUtf8()));
    }
    static Message getFromSocket(QTcpSocket *&socket){
        Message newm;
        newm.read(socket);
        return newm;
    }
    void set_json(QJsonDocument json){
        data = json.toJson();
    }
    bool read(QTcpSocket *&socket){
        QDataStream stream(socket);
        stream.setVersion(QDataStream::Qt_5_0);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint32 length = 0;

        if (socket->bytesAvailable() < sizeof(quint32))
            return 0;
        stream >> length;

        //stream >> this->data;

        this->data.resize(length);
        if (socket->bytesAvailable() < length)
            return 0;

        stream.readRawData(this->data.data(), length);
        if (socket->bytesAvailable()!=0){
            emit socket->readyRead();
        }

        return 1;

    }

    bool isEmpty(){
        return data.isEmpty();
    }


    void send_to(QTcpSocket *socket){
        QDataStream outputStream(socket);
        outputStream.setVersion(QDataStream::Qt_5_0);
        outputStream.setByteOrder(QDataStream::LittleEndian);
        auto msg = pack();
        outputStream << msg;
    }
    QString get_json_string(){
        return QString::fromUtf8(data);
    }

    QJsonDocument get_data()
    {
        return QJsonDocument::fromJson(data);
    }
    QByteArray pack()
    {
        return this->data;
    }
    explicit operator bool() const
    { return !this->data.isEmpty(); }
};

class ServerMessages{
public:
    static const Message ConnectDecline(){
        QJsonObject answer;
        answer.insert("method", QJsonValue("connect"));
        answer.insert("status", QJsonValue("error"));
        answer.insert("reason", QJsonValue("party_full"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    };
    static const Message ConnectAccept(){
        QJsonObject answer;
        answer.insert("method", QJsonValue("connect"));
        answer.insert("status", QJsonValue("success"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    };
    static const Message FieldAnswer(SetFieldResult result){
        QJsonObject answer;
        answer.insert("method", QJsonValue("set_field"));
        if (result.ok())
            answer.insert("status", QJsonValue("ok"));
        else{
            answer.insert("status", QJsonValue("error"));

            QJsonObject extra;
            extra.insert("field_size_error", QJsonValue(result.field_size_err));
            extra.insert("not_enough_digits_error", QJsonValue(result.not_enough_digits));
            extra.insert("extra_digits_error", QJsonValue(result.extra_digits));
            extra.insert("neighbours_error", QJsonValue(result.neighbours));

            QJsonArray errors;
            foreach(auto err, result.errors){
                QJsonArray coord;
                coord.append(QJsonValue(err.i));
                coord.append(QJsonValue(err.j));
                errors.append(coord);
            }
            extra.insert("errors", QJsonValue(errors));

            answer.insert("extra", QJsonValue(extra));
        }
        QJsonDocument json(answer);
        return Message::from_json(json);
    };
    static const Message OpenCellAnswer(int i, int j, int val){
        QJsonObject answer;
        answer.insert("method", QJsonValue("open_cell"));
        answer.insert("i", QJsonValue(i));
        answer.insert("j", QJsonValue(j));
        answer.insert("val", QJsonValue(val));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }

    static const Message EventOpponentLeft(){
        QJsonObject answer;
        answer.insert("event", QJsonValue("opponent_left"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }
    static const Message EventStartingUp(){
        QJsonObject answer;
        answer.insert("event", QJsonValue("starting_up"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }
    static const Message EventGameStarted(int N, int M, int K){
        QJsonObject answer;
        answer.insert("event", QJsonValue("game_started"));
        answer.insert("field_size", QJsonValue(N));
        answer.insert("numbers", QJsonValue(M));
        answer.insert("turns", QJsonValue(K));

        QJsonDocument json(answer);
        return Message::from_json(json);
    }
    static const Message EventOpponentReady(){
        QJsonObject answer;
        answer.insert("event", QJsonValue("opponent_ready"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }
    static const Message EventNextTurn(int scores1, int scores2, int turns, bool now_turn){
        QJsonObject answer;
        answer.insert("event", QJsonValue("next_turn"));
        answer.insert("scores1", QJsonValue(scores1));
        answer.insert("scores2", QJsonValue(scores2));
        answer.insert("turns", QJsonValue(turns));
        answer.insert("now_turn", QJsonValue(now_turn));

        QJsonDocument json(answer);
        return Message::from_json(json);
    }
    static const Message EventWin(bool player_win, int scores1, int scores2){
        QJsonObject answer;
        answer.insert("event", QJsonValue("win"));
        answer.insert("who", QJsonValue(player_win));
        answer.insert("scores1", QJsonValue(scores1));
        answer.insert("scores2", QJsonValue(scores2));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }
};



class ClientMessages{
public:
    static const Message connect_timestamps(quint64 time){
        QJsonObject answer;
        answer.insert("method", QJsonValue("timestamps"));
        answer.insert("time", QJsonValue::fromVariant(QVariant(time)));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }
    static const Message set_field(QVector<QVector<int>> field){
        QJsonObject answer;
        QJsonArray field_json;
        foreach (auto row, field) {
            QJsonArray row_json;
            foreach(auto num, row){
                row_json.append(QJsonValue(num));
            }
            field_json.append(QJsonValue(row_json));
        }


        answer.insert("method", QJsonValue("set_field"));
        answer.insert("field", QJsonValue(field_json));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }
    static const Message open_cell(int i, int j){
        QJsonObject answer;
        answer.insert("method", QJsonValue("open_cell"));
        answer.insert("i", QJsonValue(i));
        answer.insert("j", QJsonValue(j));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }

};

