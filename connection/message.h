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
        length = quint16(data.size());
    }
    bool read(QTcpSocket *socket){
        QDataStream stream(socket);
        stream.setVersion(QDataStream::Qt_5_0);
        stream.setByteOrder(QDataStream::BigEndian);

        QByteArray l;
        QByteArray arr;

        if (socket->bytesAvailable() < sizeof(quint16)){
            return 1;
        }
        l.resize(sizeof(quint16));
        stream.readRawData(l.data(), sizeof(quint16));
        length = l.toUInt();

        arr.resize(length);
        stream.readRawData(arr.data(), length);
        data = arr;

        //stream.writeBytes(this->data, this->length);

        return 0;
    }

    bool isEmpty(){
        return data.isEmpty();
    }


    void send_to(QTcpSocket *socket){
        QDataStream outputStream(socket);
        outputStream << pack();
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
        QByteArray msg;
        msg.clear();
        QDataStream outputStream(&msg, QIODevice::WriteOnly);
        outputStream.setVersion(QDataStream::Qt_5_0);
        outputStream.setByteOrder(QDataStream::BigEndian);
        outputStream << this->length;
        msg.append(this->data);
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
