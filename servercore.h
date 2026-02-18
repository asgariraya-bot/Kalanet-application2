#ifndef SERVERCORE_H
#define SERVERCORE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QByteArray>
#include <QString>

#include "jsonhandler.h"

class ServerCore : public QObject
{
    Q_OBJECT

public:
    explicit ServerCore(QObject *parent = nullptr);
    bool start(quint16 port);

private slots:
    void onNewConnection();
    void onClientReadyRead();
    void onClientDisconnected();
    void onSocketError(QAbstractSocket::SocketError);

private:
    QTcpServer server;
    QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*, QByteArray> buffers;
    JsonHandler handler;

    void processBuffer(QTcpSocket *socket);
    void sendJson(QTcpSocket *socket, const QJsonObject &obj);
    void log(const QString &msg);
};

#endif
