#include "servercore.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>

ServerCore::ServerCore(QObject *parent)
    : QObject(parent)
{
}

bool ServerCore::start(quint16 port)
{
    if (!server.listen(QHostAddress::Any, port))
        return false;

    connect(&server, &QTcpServer::newConnection, this, &ServerCore::onNewConnection);
    return true;
}

void ServerCore::onNewConnection()
{
    while (server.hasPendingConnections()) {
        QTcpSocket *socket = server.nextPendingConnection();
        clients.insert(socket);
        buffers[socket].clear();

        connect(socket, &QTcpSocket::readyRead, this, &ServerCore::onClientReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &ServerCore::onClientDisconnected);
        connect(socket, &QTcpSocket::errorOccurred, this, &ServerCore::onSocketError);

        log("Client connected: " + socket->peerAddress().toString());
    }
}

void ServerCore::onClientReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray &buf = buffers[socket];
    buf.append(socket->readAll());

    processBuffer(socket);
}

void ServerCore::processBuffer(QTcpSocket *socket)
{
    QByteArray &buf = buffers[socket];

    while (true) {
        int idx = buf.indexOf('\n');
        if (idx < 0)
            break;

        QByteArray line = buf.left(idx);
        buf.remove(0, idx + 1);

        line = line.trimmed();
        if (line.isEmpty())
            continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject())
            continue;

        QJsonObject req = doc.object();
        QJsonObject res = handler.handleRequest(req);

        sendJson(socket, res);
    }
}

void ServerCore::sendJson(QTcpSocket *socket, const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');
    socket->write(data);
    socket->flush();
}

void ServerCore::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    clients.remove(socket);
    buffers.remove(socket);

    log("Client disconnected: " + socket->peerAddress().toString());

    socket->deleteLater();
}

void ServerCore::onSocketError(QAbstractSocket::SocketError)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    log("Socket error: " + socket->errorString());
}

void ServerCore::log(const QString &msg)
{
    QString t = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "[" << t << "]" << msg;
}
