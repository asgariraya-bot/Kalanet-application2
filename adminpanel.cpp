#include "adminpanel.h"
#include "ui_adminpanel.h"
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

AdminPanel::AdminPanel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminPanel)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
    , serverIp(DEFAULT_SERVER_IP)
    , serverPort(DEFAULT_SERVER_PORT)
    , pendingModel(new QStandardItemModel(this))
    , approvedModel(new QStandardItemModel(this))
    , rejectedModel(new QStandardItemModel(this))
    , pendingRequest(PendingRequest::None)
    , selectedAdId(-1)
{
    ui->setupUi(this);
    setWindowTitle("Admin Panel");

    setupUiDesign();
    setupModels();

    connect(socket, &QTcpSocket::connected,    this, &AdminPanel::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &AdminPanel::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &AdminPanel::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &AdminPanel::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        socket->abort();
        QMessageBox::critical(this, "Network", "Timeout");
        emit networkError("Timeout");
        pendingRequest = PendingRequest::None;
    });

    requestPendingAds();
    requestApprovedAds();
    requestRejectedAds();
    requestStats();
}

AdminPanel::~AdminPanel()
{
    delete ui;
}

void AdminPanel::setServerAddress(const QString &ip, quint16 port)
{
    serverIp   = ip;
    serverPort = port;
}

void AdminPanel::setCurrentAdmin(const QString &username)
{
    adminUsername = username;
}

void AdminPanel::setupUiDesign()
{
    this->setStyleSheet(
        "QMainWindow { background-color: #1f1f2f; }"
        "QLabel { color: white; }"
        "QTableView { background-color: #2a2a3a; color: white; gridline-color: #444; }"
        "QHeaderView::section { background-color: #3b3b5c; color: white; }"
        "QPushButton { background-color: #3b3b5c; color: white; border-radius: 8px; padding: 6px 12px; }"
        "QPushButton:hover { background-color: #50507a; }"
        "QPushButton#approveButton { background-color: #1f7a3a; }"
        "QPushButton#approveButton:hover { background-color: #25994a; }"
        "QPushButton#rejectButton { background-color: #8b1e3f; }"
        "QPushButton#rejectButton:hover { background-color: #b32653; }"
        );
}

void AdminPanel::setupModels()
{
    pendingModel->setColumnCount(5);
    pendingModel->setHeaderData(0, Qt::Horizontal, "ID");
    pendingModel->setHeaderData(1, Qt::Horizontal, "Title");
    pendingModel->setHeaderData(2, Qt::Horizontal, "Price");
    pendingModel->setHeaderData(3, Qt::Horizontal, "Category");
    pendingModel->setHeaderData(4, Qt::Horizontal, "Owner");
    ui->pendingTableView->setModel(pendingModel);
    ui->pendingTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pendingTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->pendingTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->pendingTableView->horizontalHeader()->setStretchLastSection(true);

    approvedModel->setColumnCount(5);
    approvedModel->setHeaderData(0, Qt::Horizontal, "ID");
    approvedModel->setHeaderData(1, Qt::Horizontal, "Title");
    approvedModel->setHeaderData(2, Qt::Horizontal, "Price");
    approvedModel->setHeaderData(3, Qt::Horizontal, "Category");
    approvedModel->setHeaderData(4, Qt::Horizontal, "Owner");
    ui->approvedTableView->setModel(approvedModel);
    ui->approvedTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->approvedTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->approvedTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->approvedTableView->horizontalHeader()->setStretchLastSection(true);

    rejectedModel->setColumnCount(5);
    rejectedModel->setHeaderData(0, Qt::Horizontal, "ID");
    rejectedModel->setHeaderData(1, Qt::Horizontal, "Title");
    rejectedModel->setHeaderData(2, Qt::Horizontal, "Price");
    rejectedModel->setHeaderData(3, Qt::Horizontal, "Category");
    rejectedModel->setHeaderData(4, Qt::Horizontal, "Owner");
    ui->rejectedTableView->setModel(rejectedModel);
    ui->rejectedTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->rejectedTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->rejectedTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->rejectedTableView->horizontalHeader()->setStretchLastSection(true);
}

void AdminPanel::connectToServer()
{
    socket->abort();
    socket->connectToHost(QHostAddress(serverIp), serverPort);
    connectionTimer->start(CONNECTION_TIMEOUT);
}

void AdminPanel::requestPendingAds()
{
    pendingRequest = PendingRequest::GetPending;
    connectToServer();
}

void AdminPanel::requestApprovedAds()
{
    pendingRequest = PendingRequest::GetApproved;
    connectToServer();
}

void AdminPanel::requestRejectedAds()
{
    pendingRequest = PendingRequest::GetRejected;
    connectToServer();
}

void AdminPanel::requestStats()
{
    pendingRequest = PendingRequest::GetStats;
    connectToServer();
}

void AdminPanel::sendApproveRequest(int adId)
{
    selectedAdId = adId;
    pendingRequest = PendingRequest::ApproveAd;
    connectToServer();
}

void AdminPanel::sendRejectRequest(int adId)
{
    selectedAdId = adId;
    pendingRequest = PendingRequest::RejectAd;
    connectToServer();
}

void AdminPanel::sendRequestForCurrentPending()
{
    QJsonObject obj;

    if (pendingRequest == PendingRequest::GetPending) {
        obj["type"] = "get_pending_ads";
    } else if (pendingRequest == PendingRequest::GetApproved) {
        obj["type"] = "get_approved_ads";
    } else if (pendingRequest == PendingRequest::GetRejected) {
        obj["type"] = "get_rejected_ads";
    } else if (pendingRequest == PendingRequest::ApproveAd) {
        obj["type"] = "approve_ad";
        obj["ad_id"] = selectedAdId;
    } else if (pendingRequest == PendingRequest::RejectAd) {
        obj["type"] = "reject_ad";
        obj["ad_id"] = selectedAdId;
    } else if (pendingRequest == PendingRequest::GetStats) {
        obj["type"] = "get_admin_stats";
    } else {
        return;
    }

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');
    socket->write(data);
    socket->flush();
}

void AdminPanel::handlePendingResponse(const QJsonObject &obj)
{
    pendingModel->removeRows(0, pendingModel->rowCount());
    QJsonArray arr = obj.value("ads").toArray();
    int row = 0;
    for (const auto &v : arr) {
        QJsonObject a = v.toObject();
        pendingModel->insertRow(row);
        pendingModel->setData(pendingModel->index(row, 0), a["id"].toInt());
        pendingModel->setData(pendingModel->index(row, 1), a["title"].toString());
        pendingModel->setData(pendingModel->index(row, 2), a["price"].toDouble());
        pendingModel->setData(pendingModel->index(row, 3), a["category"].toString());
        pendingModel->setData(pendingModel->index(row, 4), a["owner"].toString());
        row++;
    }
}

void AdminPanel::handleApprovedResponse(const QJsonObject &obj)
{
    approvedModel->removeRows(0, approvedModel->rowCount());
    QJsonArray arr = obj.value("ads").toArray();
    int row = 0;
    for (const auto &v : arr) {
        QJsonObject a = v.toObject();
        approvedModel->insertRow(row);
        approvedModel->setData(approvedModel->index(row, 0), a["id"].toInt());
        approvedModel->setData(approvedModel->index(row, 1), a["title"].toString());
        approvedModel->setData(approvedModel->index(row, 2), a["price"].toDouble());
        approvedModel->setData(approvedModel->index(row, 3), a["category"].toString());
        approvedModel->setData(approvedModel->index(row, 4), a["owner"].toString());
        row++;
    }
}

void AdminPanel::handleRejectedResponse(const QJsonObject &obj)
{
    rejectedModel->removeRows(0, rejectedModel->rowCount());
    QJsonArray arr = obj.value("ads").toArray();
    int row = 0;
    for (const auto &v : arr) {
        QJsonObject a = v.toObject();
        rejectedModel->insertRow(row);
        rejectedModel->setData(rejectedModel->index(row, 0), a["id"].toInt());
        rejectedModel->setData(rejectedModel->index(row, 1), a["title"].toString());
        rejectedModel->setData(rejectedModel->index(row, 2), a["price"].toDouble());
        rejectedModel->setData(rejectedModel->index(row, 3), a["category"].toString());
        rejectedModel->setData(rejectedModel->index(row, 4), a["owner"].toString());
        row++;
    }
}

void AdminPanel::handleApproveResponse(const QJsonObject &obj)
{
    bool success = obj.value("success").toBool();
    QString msg = obj.value("message").toString();
    if (success) {
        QMessageBox::information(this, "Approve", msg);
        requestPendingAds();
        requestApprovedAds();
    } else {
        QMessageBox::critical(this, "Approve", msg);
    }
}

void AdminPanel::handleRejectResponse(const QJsonObject &obj)
{
    bool success = obj.value("success").toBool();
    QString msg = obj.value("message").toString();
    if (success) {
        QMessageBox::information(this, "Reject", msg);
        requestPendingAds();
        requestRejectedAds();
    } else {
        QMessageBox::critical(this, "Reject", msg);
    }
}

void AdminPanel::handleStatsResponse(const QJsonObject &obj)
{
    int totalUsers = obj.value("total_users").toInt();
    int totalAds   = obj.value("total_ads").toInt();
    int pending    = obj.value("pending_ads").toInt();
    int approved   = obj.value("approved_ads").toInt();
    int rejected   = obj.value("rejected_ads").toInt();

    ui->totalUsersLabel->setText(QString::number(totalUsers));
    ui->totalAdsLabel->setText(QString::number(totalAds));
    ui->pendingLabel->setText(QString::number(pending));
    ui->approvedLabel->setText(QString::number(approved));
    ui->rejectedLabel->setText(QString::number(rejected));
}

void AdminPanel::on_refreshButton_clicked()
{
    requestPendingAds();
    requestApprovedAds();
    requestRejectedAds();
    requestStats();
}

void AdminPanel::on_approveButton_clicked()
{
    QModelIndex idx = ui->pendingTableView->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Approve", "Select an ad");
        return;
    }
    int row = idx.row();
    int adId = pendingModel->data(pendingModel->index(row, 0)).toInt();
    sendApproveRequest(adId);
}

void AdminPanel::on_rejectButton_clicked()
{
    QModelIndex idx = ui->pendingTableView->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Reject", "Select an ad");
        return;
    }
    int row = idx.row();
    int adId = pendingModel->data(pendingModel->index(row, 0)).toInt();
    sendRejectRequest(adId);
}

void AdminPanel::on_tabWidget_currentChanged(int index)
{
    if (index == 0) requestPendingAds();
    else if (index == 1) requestApprovedAds();
    else if (index == 2) requestRejectedAds();
    else if (index == 3) requestStats();
}

void AdminPanel::onConnected()
{
    sendRequestForCurrentPending();
}

void AdminPanel::onDisconnected()
{
}

void AdminPanel::onReadyRead()
{
    QByteArray allData = socket->readAll();
    QList<QByteArray> messages = allData.split('\n');

    for (auto msg : messages) {
        msg = msg.trimmed();
        if (msg.isEmpty()) continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(msg, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) continue;

        QJsonObject obj = doc.object();
        QString type = obj.value("type").toString();

        if (type == "get_pending_ads_response") {
            handlePendingResponse(obj);
        } else if (type == "get_approved_ads_response") {
            handleApprovedResponse(obj);
        } else if (type == "get_rejected_ads_response") {
            handleRejectedResponse(obj);
        } else if (type == "approve_ad_response") {
            handleApproveResponse(obj);
        } else if (type == "reject_ad_response") {
            handleRejectResponse(obj);
        } else if (type == "get_admin_stats_response") {
            handleStatsResponse(obj);
        }
    }

    pendingRequest = PendingRequest::None;
}

void AdminPanel::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    QString err = socket->errorString();
    QMessageBox::critical(this, "Network", err);
    emit networkError(err);
    pendingRequest = PendingRequest::None;
}
