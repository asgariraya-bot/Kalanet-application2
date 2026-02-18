#include "addadwindow.h"
#include "ui_addadwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QBuffer>
#include <QDebug>

namespace {
const QString DEFAULT_SERVER_IP   = "127.0.0.1";
const quint16 DEFAULT_SERVER_PORT = 4545;
const int     CONNECTION_TIMEOUT  = 5000;
}

AddAdWindow::AddAdWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddAdWindow)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
    , serverIp(DEFAULT_SERVER_IP)
    , serverPort(DEFAULT_SERVER_PORT)
{
    ui->setupUi(this);
    setWindowTitle("KalaNet - Add New Ad");

    setupUiDesign();

    // Network
    connect(socket, &QTcpSocket::connected,    this, &AddAdWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &AddAdWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &AddAdWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &AddAdWindow::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        socket->abort();
        QMessageBox::critical(this, "Network", "Server did not respond in time.");
        emit networkError("Timeout in AddAdWindow");
    });
}

AddAdWindow::~AddAdWindow()
{
    delete ui;
}

void AddAdWindow::setServerAddress(const QString &ip, quint16 port)
{
    serverIp   = ip;
    serverPort = port;
}

void AddAdWindow::setCurrentUser(const QString &username)
{
    currentUsername = username;
}

void AddAdWindow::setupUiDesign()
{
    this->setStyleSheet(
        "QMainWindow { background-color: #222233; }"
        "QLabel { color: white; }"
        "QLineEdit, QTextEdit, QComboBox { "
        "  background-color: #333344; "
        "  color: white; "
        "  border-radius: 6px; "
        "}"
        "QPushButton { "
        "  background-color: #3b3b5c; "
        "  color: white; "
        "  border-radius: 8px; "
        "  padding: 6px 12px; "
        "}"
        "QPushButton:hover { background-color: #50507a; }"
        );

    ui->priceLineEdit->setPlaceholderText("Price > 0");
    ui->titleLineEdit->setPlaceholderText("Ad title");
    ui->descriptionTextEdit->setPlaceholderText("Describe your item/service...");
    ui->categoryComboBox->addItems({"Electronics", "Home", "Car", "Service", "Other"});
}

bool AddAdWindow::validateInputs(QString &errorMessage) const
{
    QString title = ui->titleLineEdit->text().trimmed();
    QString desc  = ui->descriptionTextEdit->toPlainText().trimmed();
    QString priceStr = ui->priceLineEdit->text().trimmed();
    QString category = ui->categoryComboBox->currentText();

    if (title.isEmpty()) {
        errorMessage = "Title must not be empty.";
        return false;
    }

    bool ok = false;
    double price = priceStr.toDouble(&ok);
    if (!ok || price <= 0) {
        errorMessage = "Price must be a positive number.";
        return false;
    }

    if (category.isEmpty()) {
        errorMessage = "Category must be selected.";
        return false;
    }

    if (desc.isEmpty()) {
        errorMessage = "Description must not be empty.";
        return false;
    }

    return true;
}

void AddAdWindow::connectToServer()
{
    socket->abort();
    socket->connectToHost(QHostAddress(serverIp), serverPort);
    connectionTimer->start(CONNECTION_TIMEOUT);
}

QByteArray AddAdWindow::loadImageAsBase64() const
{
    if (selectedImagePath.isEmpty())
        return QByteArray();

    QFile file(selectedImagePath);
    if (!file.open(QIODevice::ReadOnly))
        return QByteArray();

    QByteArray data = file.readAll();
    return data.toBase64();
}

void AddAdWindow::sendAddAdRequest()
{
    QString title      = ui->titleLineEdit->text().trimmed();
    QString desc       = ui->descriptionTextEdit->toPlainText().trimmed();
    QString priceStr   = ui->priceLineEdit->text().trimmed();
    QString category   = ui->categoryComboBox->currentText();
    QByteArray imgBase64 = loadImageAsBase64();

    QJsonObject obj;
    obj["type"]        = "add_ad";
    obj["owner"]       = currentUsername;
    obj["title"]       = title;
    obj["description"] = desc;
    obj["price"]       = priceStr.toDouble();
    obj["category"]    = category;
    obj["status"]      = "Pending";
    obj["image_base64"]= QString::fromUtf8(imgBase64);

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    socket->flush();
}



void AddAdWindow::on_browseImageButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Image",
        QString(),
        "Images (*.png *.jpg *.jpeg *.bmp)"
        );

    if (!fileName.isEmpty()) {
        selectedImagePath = fileName;
        ui->imagePathLabel->setText(fileName);
    }
}

void AddAdWindow::on_submitButton_clicked()
{
    QString error;
    if (!validateInputs(error)) {
        QMessageBox::warning(this, "Invalid input", error);
        return;
    }

    if (currentUsername.isEmpty()) {
        QMessageBox::warning(this, "Error", "Current user is not set.");
        return;
    }

    connectToServer();
    sendAddAdRequest();
}

void AddAdWindow::on_cancelButton_clicked()
{
    close();
}



void AddAdWindow::onConnected()
{

}

void AddAdWindow::onDisconnected()
{
    // Optional
}

void AddAdWindow::onReadyRead()
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

        if (type == "add_ad_response") {
            bool success = obj["success"].toBool();
            QString message = obj["message"].toString();

            if (success) {
                QMessageBox::information(this, "Ad created",
                                         message.isEmpty() ? "Ad created successfully and is pending approval." : message);
                emit adCreated();
                close();
            } else {
                QMessageBox::critical(this, "Error",
                                      message.isEmpty() ? "Failed to create ad." : message);
            }
        }
    }
}

void AddAdWindow::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    QString err = socket->errorString();
    QMessageBox::critical(this, "Network error", err);
    emit networkError(err);
}
