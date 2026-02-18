#include "profilewindow.h"
#include "ui_profilewindow.h"
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

ProfileWindow::ProfileWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProfileWindow)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
    , serverIp(DEFAULT_SERVER_IP)
    , serverPort(DEFAULT_SERVER_PORT)
    , userAdsModel(new QStandardItemModel(this))
    , purchasesModel(new QStandardItemModel(this))
    , salesModel(new QStandardItemModel(this))
    , pendingRequest(PendingRequest::None)
{
    ui->setupUi(this);
    setWindowTitle("Profile");

    setupUiDesign();
    setupModels();

    connect(socket, &QTcpSocket::connected,    this, &ProfileWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &ProfileWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &ProfileWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &ProfileWindow::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        socket->abort();
        QMessageBox::critical(this, "Network", "Timeout");
        emit networkError("Timeout");
        pendingRequest = PendingRequest::None;
    });

    requestProfile();
    requestUserAds();
    requestPurchases();
    requestSales();
}

ProfileWindow::~ProfileWindow()
{
    delete ui;
}

void ProfileWindow::setServerAddress(const QString &ip, quint16 port)
{
    serverIp   = ip;
    serverPort = port;
}

void ProfileWindow::setCurrentUser(const QString &username)
{
    currentUsername = username;
}

void ProfileWindow::setupUiDesign()
{
    this->setStyleSheet(
        "QMainWindow { background-color: #1f1f2f; }"
        "QLabel { color: white; }"
        "QLabel#nameLabel { font-weight: bold; font-size: 18px; }"
        "QLineEdit { background-color: #333344; color: white; border-radius: 6px; }"
        "QTableView { background-color: #2a2a3a; color: white; gridline-color: #444; }"
        "QHeaderView::section { background-color: #3b3b5c; color: white; }"
        "QPushButton { background-color: #3b3b5c; color: white; border-radius: 8px; padding: 6px 12px; }"
        "QPushButton:hover { background-color: #50507a; }"
        );
}

void ProfileWindow::setupModels()
{
    userAdsModel->setColumnCount(5);
    userAdsModel->setHeaderData(0, Qt::Horizontal, "ID");
    userAdsModel->setHeaderData(1, Qt::Horizontal, "Title");
    userAdsModel->setHeaderData(2, Qt::Horizontal, "Price");
    userAdsModel->setHeaderData(3, Qt::Horizontal, "Category");
    userAdsModel->setHeaderData(4, Qt::Horizontal, "Status");
    ui->userAdsTableView->setModel(userAdsModel);
    ui->userAdsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->userAdsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->userAdsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->userAdsTableView->horizontalHeader()->setStretchLastSection(true);

    purchasesModel->setColumnCount(4);
    purchasesModel->setHeaderData(0, Qt::Horizontal, "Title");
    purchasesModel->setHeaderData(1, Qt::Horizontal, "Price");
    purchasesModel->setHeaderData(2, Qt::Horizontal, "Date");
    purchasesModel->setHeaderData(3, Qt::Horizontal, "Seller");
    ui->purchasesTableView->setModel(purchasesModel);
    ui->purchasesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->purchasesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->purchasesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->purchasesTableView->horizontalHeader()->setStretchLastSection(true);

    salesModel->setColumnCount(4);
    salesModel->setHeaderData(0, Qt::Horizontal, "Title");
    salesModel->setHeaderData(1, Qt::Horizontal, "Price");
    salesModel->setHeaderData(2, Qt::Horizontal, "Date");
    salesModel->setHeaderData(3, Qt::Horizontal, "Buyer");
    ui->salesTableView->setModel(salesModel);
    ui->salesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->salesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->salesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->salesTableView->horizontalHeader()->setStretchLastSection(true);
}

void ProfileWindow::connectToServer()
{
    socket->abort();
    socket->connectToHost(QHostAddress(serverIp), serverPort);
    connectionTimer->start(CONNECTION_TIMEOUT);
}

void ProfileWindow::requestProfile()
{
    pendingRequest = PendingRequest::GetProfile;
    connectToServer();
}

void ProfileWindow::requestUserAds()
{
    pendingRequest = PendingRequest::GetUserAds;
    connectToServer();
}

void ProfileWindow::requestPurchases()
{
    pendingRequest = PendingRequest::GetPurchases;
    connectToServer();
}

void ProfileWindow::requestSales()
{
    pendingRequest = PendingRequest::GetSales;
    connectToServer();
}

void ProfileWindow::sendRequestForCurrentPending()
{
    QJsonObject obj;

    if (pendingRequest == PendingRequest::GetProfile) {
        obj["type"]     = "get_profile";
        obj["username"] = currentUsername;
    } else if (pendingRequest == PendingRequest::GetUserAds) {
        obj["type"]     = "get_user_ads";
        obj["username"] = currentUsername;
    } else if (pendingRequest == PendingRequest::GetPurchases) {
        obj["type"]     = "get_user_purchases";
        obj["username"] = currentUsername;
    } else if (pendingRequest == PendingRequest::GetSales) {
        obj["type"]     = "get_user_sales";
        obj["username"] = currentUsername;
    } else {
        connectionTimer->stop();
        socket->disconnectFromHost();
        return;
    }

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    socket->flush();
}

void ProfileWindow::handleProfileResponse(const QJsonObject &obj)
{
    QString name       = obj.value("name").toString();
    QString email      = obj.value("email").toString();
    QString phone      = obj.value("phone").toString();
    QString joinDate   = obj.value("join_date").toString();
    int adsCount       = obj.value("ads_count").toInt();
    int purchasesCount = obj.value("purchases").toInt();
    int salesCount     = obj.value("sales").toInt();

    ui->nameLabel->setText(name);
    ui->usernameLabel->setText(currentUsername);
    ui->emailLabel->setText(email);
    ui->phoneLabel->setText(phone);
    ui->joinDateLabel->setText(joinDate);
    ui->adsCountLabel->setText(QString::number(adsCount));
    ui->purchasesCountLabel->setText(QString::number(purchasesCount));
    ui->salesCountLabel->setText(QString::number(salesCount));
}

void ProfileWindow::handleUserAdsResponse(const QJsonObject &obj)
{
    userAdsModel->removeRows(0, userAdsModel->rowCount());

    QJsonArray arr = obj.value("ads").toArray();
    int row = 0;
    for (const auto &v : arr) {
        if (!v.isObject()) continue;
        QJsonObject a = v.toObject();

        int id          = a.value("id").toInt();
        QString title   = a.value("title").toString();
        double price    = a.value("price").toDouble();
        QString category= a.value("category").toString();
        QString status  = a.value("status").toString();

        userAdsModel->insertRow(row);
        userAdsModel->setData(userAdsModel->index(row, 0), id);
        userAdsModel->setData(userAdsModel->index(row, 1), title);
        userAdsModel->setData(userAdsModel->index(row, 2), price);
        userAdsModel->setData(userAdsModel->index(row, 3), category);
        userAdsModel->setData(userAdsModel->index(row, 4), status);
        ++row;
    }
}

void ProfileWindow::handlePurchasesResponse(const QJsonObject &obj)
{
    purchasesModel->removeRows(0, purchasesModel->rowCount());

    QJsonArray arr = obj.value("purchases").toArray();
    int row = 0;
    for (const auto &v : arr) {
        if (!v.isObject()) continue;
        QJsonObject p = v.toObject();

        QString title     = p.value("title").toString();
        double price      = p.value("price").toDouble();
        QString date      = p.value("date").toString();
        QString seller    = p.value("seller").toString();

        purchasesModel->insertRow(row);
        purchasesModel->setData(purchasesModel->index(row, 0), title);
        purchasesModel->setData(purchasesModel->index(row, 1), price);
        purchasesModel->setData(purchasesModel->index(row, 2), date);
        purchasesModel->setData(purchasesModel->index(row, 3), seller);
        ++row;
    }
}

void ProfileWindow::handleSalesResponse(const QJsonObject &obj)
{
    salesModel->removeRows(0, salesModel->rowCount());

    QJsonArray arr = obj.value("sales").toArray();
    int row = 0;
    for (const auto &v : arr) {
        if (!v.isObject()) continue;
        QJsonObject s = v.toObject();

        QString title   = s.value("title").toString();
        double price    = s.value("price").toDouble();
        QString date    = s.value("date").toString();
        QString buyer   = s.value("buyer").toString();

        salesModel->insertRow(row);
        salesModel->setData(salesModel->index(row, 0), title);
        salesModel->setData(salesModel->index(row, 1), price);
        salesModel->setData(salesModel->index(row, 2), date);
        salesModel->setData(salesModel->index(row, 3), buyer);
        ++row;
    }
}

void ProfileWindow::on_refreshButton_clicked()
{
    requestProfile();
    requestUserAds();
    requestPurchases();
    requestSales();
}

void ProfileWindow::onConnected()
{
    sendRequestForCurrentPending();
}

void ProfileWindow::onDisconnected()
{
}

void ProfileWindow::onReadyRead()
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
        QString type = obj.value("type").toString();

        if (type == "get_profile_response") {
            handleProfileResponse(obj);
        } else if (type == "get_user_ads_response") {
            handleUserAdsResponse(obj);
        } else if (type == "get_user_purchases_response") {
            handlePurchasesResponse(obj);
        } else if (type == "get_user_sales_response") {
            handleSalesResponse(obj);
        }
    }

    pendingRequest = PendingRequest::None;
}

void ProfileWindow::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    QString err = socket->errorString();
    QMessageBox::critical(this, "Network", err);
    emit networkError(err);
    pendingRequest = PendingRequest::None;
}
