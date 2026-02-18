#include "cartwindow.h"
#include "ui_cartwindow.h"

#include <QMessageBox>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItem>

namespace {
const QString DEFAULT_SERVER_IP   = "127.0.0.1";
const quint16 DEFAULT_SERVER_PORT = 4545;
const int     CONNECTION_TIMEOUT  = 5000;
}

CartWindow::CartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CartWindow)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
    , serverIp(DEFAULT_SERVER_IP)
    , serverPort(DEFAULT_SERVER_PORT)
    , model(new QStandardItemModel(this))
    , totalPrice(0.0)
{
    ui->setupUi(this);
    setWindowTitle("Cart");

    setupUiDesign();
    setupModel();

    connect(socket, &QTcpSocket::connected,    this, &CartWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &CartWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &CartWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &CartWindow::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        socket->abort();
        QMessageBox::critical(this, "Network", "Timeout");
        emit networkError("Timeout");
    });

    requestCart();
}

CartWindow::~CartWindow()
{
    delete ui;
}

void CartWindow::setServerAddress(const QString &ip, quint16 port)
{
    serverIp   = ip;
    serverPort = port;
}

void CartWindow::setCurrentUser(const QString &username)
{
    currentUsername = username;
}

void CartWindow::setupUiDesign()
{
    this->setStyleSheet(
        "QMainWindow { background-color: #1f1f2f; }"
        "QLabel { color: white; }"
        "QLineEdit { background-color: #333344; color: white; border-radius: 6px; }"
        "QTableView { background-color: #2a2a3a; color: white; gridline-color: #444; }"
        "QHeaderView::section { background-color: #3b3b5c; color: white; }"
        "QPushButton { background-color: #3b3b5c; color: white; border-radius: 8px; padding: 6px 12px; }"
        "QPushButton:hover { background-color: #50507a; }"
        "QPushButton#purchaseButton { background-color: #1f7a3a; }"
        "QPushButton#purchaseButton:hover { background-color: #25994a; }"
        "QPushButton#removeButton { background-color: #8b1e3f; }"
        "QPushButton#removeButton:hover { background-color: #b32653; }"
        );
    ui->totalPriceLineEdit->setReadOnly(true);
}

void CartWindow::setupModel()
{
    model->setColumnCount(4);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Title");
    model->setHeaderData(2, Qt::Horizontal, "Category");
    model->setHeaderData(3, Qt::Horizontal, "Price");

    ui->cartTableView->setModel(model);
    ui->cartTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->cartTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->cartTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->cartTableView->horizontalHeader()->setStretchLastSection(true);
}

void CartWindow::connectToServer()
{
    socket->abort();
    socket->connectToHost(QHostAddress(serverIp), serverPort);
    connectionTimer->start(CONNECTION_TIMEOUT);
}

void CartWindow::requestCart()
{
    connectToServer();
}

void CartWindow::sendRemoveRequest(int adId)
{
    QJsonObject obj;
    obj["type"]     = "remove_from_cart";
    obj["username"] = currentUsername;
    obj["ad_id"]    = adId;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    socket->flush();
}

void CartWindow::sendPurchaseRequest()
{
    QJsonObject obj;
    obj["type"]     = "purchase_cart";
    obj["username"] = currentUsername;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    socket->flush();
}

void CartWindow::updateTotalPrice()
{
    totalPrice = 0.0;
    for (const auto &it : items)
        totalPrice += it.price;

    ui->totalPriceLineEdit->setText(QString::number(totalPrice, 'f', 2));
}

void CartWindow::populateTable(const QList<CartItem> &list)
{
    model->removeRows(0, model->rowCount());

    int row = 0;
    for (const auto &it : list) {
        model->insertRow(row);
        model->setData(model->index(row, 0), it.id);
        model->setData(model->index(row, 1), it.title);
        model->setData(model->index(row, 2), it.category);
        model->setData(model->index(row, 3), it.price);
        ++row;
    }

    updateTotalPrice();
}

void CartWindow::on_refreshButton_clicked()
{
    requestCart();
}

void CartWindow::on_removeButton_clicked()
{
    QModelIndex idx = ui->cartTableView->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "No selection", "Select an item");
        return;
    }

    int row = idx.row();
    int adId = model->data(model->index(row, 0)).toInt();

    sendRemoveRequest(adId);
}

void CartWindow::on_purchaseButton_clicked()
{
    if (items.isEmpty()) {
        QMessageBox::warning(this, "Empty cart", "Cart is empty");
        return;
    }

    sendPurchaseRequest();
}

void CartWindow::onConnected()
{
    QJsonObject obj;
    obj["type"]     = "get_cart";
    obj["username"] = currentUsername;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    socket->flush();
}

void CartWindow::onDisconnected()
{
}

void CartWindow::onReadyRead()
{
    QByteArray allData = socket->readAll();
    QList<QByteArray> messages = allData.split('\n');

    for (auto msg : messages) {
        msg = msg.trimmed();
        if (msg.isEmpty())
            continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(msg, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject())
            continue;

        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();

        if (type == "get_cart_response") {
            items.clear();
            QJsonArray arr = obj["items"].toArray();
            for (const auto &v : arr) {
                if (!v.isObject()) continue;
                QJsonObject a = v.toObject();

                CartItem it;
                it.id       = a["id"].toInt();
                it.title    = a["title"].toString();
                it.category = a["category"].toString();
                it.price    = a["price"].toDouble();
                items.append(it);
            }
            populateTable(items);
            emit cartUpdated();
        } else if (type == "remove_from_cart_response") {
            bool success = obj["success"].toBool();
            QString message = obj["message"].toString();
            if (success) {
                requestCart();
            } else {
                QMessageBox::critical(this, "Error", message);
            }
        } else if (type == "purchase_cart_response") {
            bool success = obj["success"].toBool();
            QString message = obj["message"].toString();
            if (success) {
                QMessageBox::information(this, "Purchase", message);
                items.clear();
                populateTable(items);
                emit purchaseCompleted();
            } else {
                QMessageBox::critical(this, "Error", message);
            }
        }
    }
}

void CartWindow::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    QString err = socket->errorString();
    QMessageBox::critical(this, "Network", err);
    emit networkError(err);
}
