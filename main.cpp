
#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    QFile stylesheet(":/styles/Darkeum.qss");
    stylesheet.open(QFile::ReadOnly);
    QString style = {QLatin1String(stylesheet.readAll())};
    a.setStyleSheet(style);


    MainWindow w;
    w.show();
    return a.exec();
}
