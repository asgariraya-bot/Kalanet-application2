#include "mainmenu.h"
#include "ui_mainmenu.h"
#include <QMessageBox>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QDebug>

namespace {
const QString DEFAULT_SERVER_IP   = "127.0.0.1";
const quint16 DEFAULT_SERVER_PORT = 4545;
const int     CONNECTION_TIMEOUT  = 5000;
}


MainMenu::MainMenu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainMenu)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
    , serverIp(DEFAULT_SERVER_IP)
    , serverPort(DEFAULT_SERVER_PORT)
    , currentRole(UserRole::NormalUser)
{
    ui->setupUi(this);
    setWindowTitle("KalaNet - Main Menu");

    setupUiDesign();
    setupButtonIcons();

    // Network connections
    connect(socket, &QTcpSocket::connected,    this, &MainMenu::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainMenu::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &MainMenu::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &MainMenu::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        socket->abort();
        QMessageBox::critical(this, "Network", "Server did not respond in time.");
        emit networkErrorOccurred("Timeout");
    });
}

MainMenu::~MainMenu()
{
    delete ui;
}


void MainMenu::setCurrentUser(const QString &username, bool isAdmin)
{
    currentUsername = username;
    currentRole     = isAdmin ? UserRole::Admin : UserRole::NormalUser;

    ui->welcomeLabel->setText(QString("Welcome, %1").arg(username));
    updateAdminVisibility();

    refreshDashboard();
}

void MainMenu::setCurrentUser(const QString &username, UserRole role)
{
    currentUsername = username;
    currentRole     = role;

    ui->welcomeLabel->setText(QString("Welcome, %1").arg(username));
    updateAdminVisibility();

    refreshDashboard();
}

void MainMenu::setServerAddress(const QString &ip, quint16 port)
{
    serverIp   = ip;
    serverPort = port;
}

void MainMenu::refreshDashboard()
{
    connectToServer();
}



void MainMenu::setupUiDesign()
{
    this->setStyleSheet(
        "QMainWindow { background-color: #1e1e2f; }"
        "QLabel#welcomeLabel { color: white; font-size: 20px; font-weight: bold; }"
        "QPushButton { "
        "  background-color: #3b3b5c; "
        "  color: white; "
        "  border-radius: 10px; "
        "  padding: 10px; "
        "  font-size: 15px; "
        "}"
        "QPushButton:hover { background-color: #50507a; }"
        "QPushButton#logoutButton { background-color: #8b1e3f; }"
        "QPushButton#logoutButton:hover { background-color: #b32653; }"
        );
}

void MainMenu::setupButtonIcons()
{
    ui->addAdButton->setIcon(QIcon(":/icons/add.png"));
    ui->browseAdsButton->setIcon(QIcon(":/icons/browse.png"));
    ui->cartButton->setIcon(QIcon(":/icons/cart.png"));
    ui->walletButton->setIcon(QIcon(":/icons/wallet.png"));
    ui->profileButton->setIcon(QIcon(":/icons/profile.png"));
    ui->adminPanelButton->setIcon(QIcon(":/icons/admin.png"));
    ui->logoutButton->setIcon(QIcon(":/icons/logout.png"));
}

void MainMenu::updateAdminVisibility()
{
    ui->adminPanelButton->setVisible(isAdmin());
}

bool MainMenu::isAdmin() const
{
    return currentRole == UserRole::Admin;
}

void MainMenu::updateStatsOnUi()
{
    ui->walletButton->setText(QString("Wallet (%1 T)").arg(stats.walletBalance));
    ui->cartButton->setText(QString("Cart (%1)").arg(stats.cartItems));
    ui->browseAdsButton->setText(QString("Browse Ads (%1)").arg(stats.totalAds));

    if (isAdmin()) {
        ui->adminPanelButton->setText(
            QString("Admin Panel (%1 pending)").arg(stats.pendingApprovalAds)
            );
    }
}


void MainMenu::connectToServer()
{
    socket->abort();
    socket->connectToHost(QHostAddress(serverIp), serverPort);
    connectionTimer->start(CONNECTION_TIMEOUT);
}

void MainMenu::sendInitialRequest()
{
    QJsonObject obj;
    obj["type"]     = "mainmenu_init";
    obj["username"] = currentUsername;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    socket->flush();
}

void MainMenu::handleInitResponse(const QJsonObject &obj)
{
    stats.totalAds           = obj.value("total_ads").toInt();
    stats.userAds            = obj.value("user_ads").toInt();
    stats.cartItems          = obj.value("cart_items").toInt();
    stats.walletBalance      = obj.value("wallet_balance").toInt();
    stats.pendingApprovalAds = obj.value("pending_ads").toInt();

    updateStatsOnUi();
    emit statsUpdated(stats);
}



void MainMenu::on_addAdButton_clicked()
{
    emit openAddAdRequested();
}

void MainMenu::on_browseAdsButton_clicked()
{
    emit openBrowseAdsRequested();
}

void MainMenu::on_cartButton_clicked()
{
    emit openCartRequested();
}

void MainMenu::on_walletButton_clicked()
{
    emit openWalletRequested();
}

void MainMenu::on_profileButton_clicked()
{
    emit openProfileRequested();
}

void MainMenu::on_adminPanelButton_clicked()
{
    if (!isAdmin()) {
        QMessageBox::warning(this, "Access denied", "You are not an admin.");
        return;
    }
    emit openAdminPanelRequested();
}

void MainMenu::on_logoutButton_clicked()
{
    emit logoutRequested();
    close();
}



void MainMenu::onConnected()
{
    sendInitialRequest();
}

void MainMenu::onDisconnected()
{
    // Optional UI update
}

void MainMenu::onReadyRead()
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

        if (type == "mainmenu_init_response") {
            handleInitResponse(obj);
        }
    }
}

void MainMenu::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    QString err = socket->errorString();
    QMessageBox::critical(this, "Network error", err);
    emit networkErrorOccurred(err);
}
