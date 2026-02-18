#include "adsbrowserwindow.h"
#include "ui_adsbrowserwindow.h"
#include <QMessageBox>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItem>
#include <QDebug>

namespace {
const QString DEFAULT_SERVER_IP   = "127.0.0.1";
const quint16 DEFAULT_SERVER_PORT = 4545;
const int     CONNECTION_TIMEOUT  = 5000;
}

AdsBrowserWindow::AdsBrowserWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdsBrowserWindow)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
    , serverIp(DEFAULT_SERVER_IP)
    , serverPort(DEFAULT_SERVER_PORT)
    , model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    setWindowTitle("KalaNet - Browse Ads");

    setupUiDesign();
    setupModel();

    // Network
    connect(socket, &QTcpSocket::connected,    this, &AdsBrowserWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &AdsBrowserWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &AdsBrowserWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &AdsBrowserWindow::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        socket->abort();
        QMessageBox::critical(this, "Network", "Server did not respond in time.");
        emit networkError("Timeout in AdsBrowserWindow");
    });

    ui->categoryFilterComboBox->addItem("All");
    ui->categoryFilterComboBox->addItems({"Electronics", "Home", "Car", "Service", "Other"});

    ui->minPriceSpinBox->setMinimum(0);
    ui->maxPriceSpinBox->setMinimum(0);
    ui->maxPriceSpinBox->setMaximum(1e9);

    requestAdsList();
}

AdsBrowserWindow::~AdsBrowserWindow()
{
    delete ui;
}

void AdsBrowserWindow::setServerAddress(const QString &ip, quint16 port)
{
    serverIp   = ip;
    serverPort = port;
}

void AdsBrowserWindow::setCurrentUser(const QString &username)
{
    currentUsername = username;
}

void AdsBrowserWindow::setupUiDesign()
{
    this->setStyleSheet(
        "QMainWindow { background-color: #202030; }"
        "QLabel { color: white; }"
        "QLineEdit, QComboBox, QDoubleSpinBox { "
        "  background-color: #333344; "
        "  color: white; "
        "  border-radius: 6px; "
        "}"
        "QTableView { "
        "  background-color: #2a2a3a; "
        "  color: white; "
        "  gridline-color: #444; "
        "}"
        "QHeaderView::section { "
        "  background-color: #3b3b5c; "
        "  color: white; "
        "}"
        "QPushButton { "
        "  background-color: #3b3b5c; "
        "  color: white; "
        "  border-radius: 8px; "
        "  padding: 6px 12px; "
        "}"
        "QPushButton:hover { background-color: #50507a; }"
        );
}

void AdsBrowserWindow::setupModel()
{
    model->setColumnCount(5);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Title");
    model->setHeaderData(2, Qt::Horizontal, "Category");
    model->setHeaderData(3, Qt::Horizontal, "Price");
    model->setHeaderData(4, Qt::Horizontal, "Status");

    ui->adsTableView->setModel(model);
    ui->adsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->adsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->adsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->adsTableView->horizontalHeader()->setStretchLastSection(true);
}

void AdsBrowserWindow::connectToServer()
{
    socket->abort();
    socket->connectToHost(QHostAddress(serverIp), serverPort);
    connectionTimer->start(CONNECTION_TIMEOUT);
}

void AdsBrowserWindow::requestAdsList()
{
    connectToServer();
}

void AdsBrowserWindow::populateTable(const QList<AdItem> &ads)
{
    model->removeRows(0, model->rowCount());

    int row = 0;
    for (const auto &ad : ads) {
        model->insertRow(row);
        model->setData(model->index(row, 0), ad.id);
        model->setData(model->index(row, 1), ad.title);
        model->setData(model->index(row, 2), ad.category);
        model->setData(model->index(row, 3), ad.price);
        model->setData(model->index(row, 4), ad.status);
        ++row;
    }
}

QList<AdsBrowserWindow::AdItem> AdsBrowserWindow::filteredAds() const
{
    QList<AdItem> result;

    QString searchText = ui->searchLineEdit->text().trimmed().toLower();
    QString category   = ui->categoryFilterComboBox->currentText();
    double minPrice    = ui->minPriceSpinBox->value();
    double maxPrice    = ui->maxPriceSpinBox->value();
    if (maxPrice <= 0) maxPrice = 1e12;

    for (const auto &ad : allAds) {
        if (!searchText.isEmpty() &&
            !ad.title.toLower().contains(searchText)) {
            continue;
        }

        if (category != "All" && ad.category != category)
            continue;

        if (ad.price < minPrice || ad.price > maxPrice)
            continue;

        result.append(ad);
    }

    return result;
}

void AdsBrowserWindow::applyFilters()
{
    populateTable(filteredAds());
}


void AdsBrowserWindow::on_searchLineEdit_textChanged(const QString &)
{
    applyFilters();
}

void AdsBrowserWindow::on_categoryFilterComboBox_currentIndexChanged(int)
{
    applyFilters();
}

void AdsBrowserWindow::on_minPriceSpinBox_valueChanged(double)
{
    applyFilters();
}

void AdsBrowserWindow::on_maxPriceSpinBox_valueChanged(double)
{
    applyFilters();
}

void AdsBrowserWindow::on_refreshButton_clicked()
{
    requestAdsList();
}

void AdsBrowserWindow::on_addToCartButton_clicked()
{
    QModelIndex idx = ui->adsTableView->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "No selection", "Please select an ad first.");
        return;
    }

    int row = idx.row();
    int adId = model->data(model->index(row, 0)).toInt();

    emit addToCartRequested(adId);
    QMessageBox::information(this, "Cart", "Ad added to cart (client-side signal).");
}



void AdsBrowserWindow::onConnected()
{
    QJsonObject obj;
    obj["type"] = "get_ads";
    obj["status"] = "Approved";

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    socket->flush();
}

void AdsBrowserWindow::onDisconnected()
{
    // Optional
}

void AdsBrowserWindow::onReadyRead()
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

        if (type == "get_ads_response") {
            allAds.clear();

            QJsonArray arr = obj["ads"].toArray();
            for (const auto &v : arr) {
                if (!v.isObject()) continue;
                QJsonObject a = v.toObject();

                AdItem item;
                item.id       = a["id"].toInt();
                item.title    = a["title"].toString();
                item.category = a["category"].toString();
                item.price    = a["price"].toDouble();
                item.status   = a["status"].toString();
                item.thumbnailBase64 = a["thumbnail_base64"].toString();

                allAds.append(item);
            }

            applyFilters();
        }
    }
}

void AdsBrowserWindow::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    QString err = socket->errorString();
    QMessageBox::critical(this, "Network error", err);
    emit networkError(err);
}
