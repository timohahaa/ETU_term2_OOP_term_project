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
        qDebug()<<socket->bytesAvailable();
        quint32 length = 0;
        stream >> length;
        qDebug() << length;
        qDebug() << socket->bytesAvailable();
        //stream >> this->data;

        this->data.resize(length);
        stream.readRawData(this->data.data(), length);


        //if (socket->bytesAvailable() < sizeof(quint64)){
        //    return 1;
        //}




//        l.resize(sizeof(quint16));
//        stream.readRawData(l.data(), sizeof(quint16));
//        length = l.toUInt();

//

        //stream.writeBytes(this->data, this->length);

        return 0;
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
        return Message::from_json(json);
    };

};
