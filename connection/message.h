#include <QMainWindow>
#include <QJsonDocument>
#include <QDataStream>
#include <QIODevice>
#include <QTcpSocket>
#include <QJsonObject>
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
        stream.setByteOrder(QDataStream::BigEndian);
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
        outputStream.setByteOrder(QDataStream::BigEndian);
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
        //QByteArray msg;
        //msg.clear();

        //QDataStream outputStream(&msg, QIODevice::WriteOnly);
        //outputStream.setVersion(QDataStream::Qt_5_0);
        //outputStream.setByteOrder(QDataStream::LittleEndian);

        //outputStream << data.; //msg.append(this->data);
        return this->data;
    }
    explicit operator bool() const
    { return !this->data.isEmpty(); }
};

class ServerMessages{
public:
    static const Message ConnectDecline(){
        QJsonObject answer;
        answer.insert("status", QJsonValue("error"));
        answer.insert("reason", QJsonValue("party_full"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    };
    static const Message ConnectAccept(){
        QJsonObject answer;
        answer.insert("status", QJsonValue("success"));
        QJsonDocument json(answer);
        return Message::from_json(json);
    };
};

class ClientMessages{
public:
    static const Message connect_handshake(quint64 time){
        QJsonObject answer;
        answer.insert("method", QJsonValue("connect"));
        answer.insert("time", QJsonValue::fromVariant(QVariant(time)));
        QJsonDocument json(answer);
        return Message::from_json(json);
    }
};

