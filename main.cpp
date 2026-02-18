#include "mainwindow.h"
#include <QCoreApplication>
#include <QDebug>
#include "servercore.h"
#include "database.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Database::instance().loadFromFile("kalanet_db.json");

    ServerCore server;
    if (!server.start(4545)) {
        qCritical() << "Server failed to start";
        return -1;
    }
    MainWindow w;
    qDebug() << "KalaNet Server started on port 4545";

    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        Database::instance().saveToFile("kalanet_db.json");
    });
     w.show();
    return a.exec();
}
