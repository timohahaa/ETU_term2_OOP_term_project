#include <QMainWindow>
#include <QJsonDocument>
#include <QDataStream>
#include <QIODevice>

enum Methods: quint16{
    checkConnection,
    addUser,
    studnum,
    lastname,
    fullname,
    group,
    selectAll,
    deleteUser,
    serverError
};


class Message{
    quint16 length;
    QByteArray data;

public:
    void from_json(QJsonDocument json)
    {
        data = json.toJson();
        length = data.size();
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
};
