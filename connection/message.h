#include <QMainWindow>
#include <QJsonDocument>
#include <QDataStream>
#include <QIODevice>
#include <QTcpSocket>
#include <QJsonObject>
class Message{
    quint16 length = 0;
    QByteArray data;

public:
    static Message from_json(QJsonDocument json)
    {
        Message newm;
        newm.data = json.toJson();
        newm.length = newm.data.size();
        return newm;
    }
    static Message from_string(QString string){
        return from_json(QJsonDocument::fromJson(string.toUtf8()));
    }
    static Message getFromSocket(QTcpSocket *socket){
        Message newm;
        newm.read(socket);
        return newm;
    }

    void set_json(QJsonDocument json){
        data = json.toJson();
        length = data.size();
    }
    bool read(QTcpSocket *socket){
        QDataStream stream(socket);


        //auto a = socket->bytesAvailable();
        if (socket->bytesAvailable() < sizeof(quint16)){
            return 1;
        }

        quint16 a;
        stream >> a;
        stream >> this->length;


        if (socket->bytesAvailable() < length) {
            return 1;
        }

        stream >> this->data;
        return 0;
    }

    bool isEmpty(){
        return data.isEmpty();
    }


    void send_to(QTcpSocket *socket){
        QDataStream outputStream(socket);
        outputStream << pack();
    }
    QJsonDocument get_data()
    {
        return QJsonDocument::fromJson(data);
    }
    QByteArray pack()
    {
        QByteArray msg;
        QDataStream outputStream(&msg, QIODevice::WriteOnly);
        outputStream << length << data;
        return msg;
    }
    explicit operator bool() const
    { return length>0; }
};

class ConnectionMessages{
public:
    static const Message PartyFullMessage(){
        QJsonObject answer;
        answer.insert("status", QJsonValue("error"));
        answer.insert("reason", QJsonValue("party_full"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    };
    static const Message ConnectAndWait(){
        QJsonObject answer;
        answer.insert("status", QJsonValue("success"));
        answer.insert("next", QJsonValue("wait"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    };
    static const Message ConnectAndStart(){
        QJsonObject answer;
        answer.insert("status", QJsonValue("success"));
        answer.insert("next", QJsonValue("start"));
        QJsonDocument json(answer);
        return Message::from_json(json);};

};
