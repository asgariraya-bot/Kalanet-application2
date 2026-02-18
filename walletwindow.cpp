#include "walletwindow.h"
#include "ui_walletwindow.h"

#include <QMessageBox>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItem>
#include <QLineEdit>

namespace {
const QString DEFAULT_SERVER_IP   = "127.0.0.1";
const quint16 DEFAULT_SERVER_PORT = 4545;
const int     CONNECTION_TIMEOUT  = 5000;
}

WalletWindow::WalletWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WalletWindow)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
    , serverIp(DEFAULT_SERVER_IP)
    , serverPort(DEFAULT_SERVER_PORT)
    , transactionsModel(new QStandardItemModel(this))
    , currentBalance(0.0)
    , pendingAction(PendingAction::None)
    , pendingAmount(0.0)
{
    ui->setupUi(this);
    setWindowTitle("Wallet");

    setupUiDesign();
    setupTransactionsModel();

    ui->balanceLineEdit->setReadOnly(true);

    connect(socket, &QTcpSocket::connected,    this, &WalletWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &WalletWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &WalletWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &WalletWindow::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        socket->abort();
        QMessageBox::critical(this, "Network", "Timeout");
        emit networkError("Timeout");
        pendingAction = PendingAction::None;
    });

    requestWallet();
    requestTransactions();
}

WalletWindow::~WalletWindow()
{
    delete ui;
}

void WalletWindow::setServerAddress(const QString &ip, quint16 port)
{
    serverIp   = ip;
    serverPort = port;
}

void WalletWindow::setCurrentUser(const QString &username)
{
    currentUsername = username;
}

void WalletWindow::setupUiDesign()
{
    this->setStyleSheet(
        "QMainWindow { background-color: #1f1f2f; }"
        "QLabel { color: white; }"
        "QLineEdit { background-color: #333344; color: white; border-radius: 6px; }"
        "QTableView { background-color: #2a2a3a; color: white; gridline-color: #444; }"
        "QHeaderView::section { background-color: #3b3b5c; color: white; }"
        "QPushButton { background-color: #3b3b5c; color: white; border-radius: 8px; padding: 6px 12px; }"
        "QPushButton:hover { background-color: #50507a; }"
        "QPushButton#depositButton { background-color: #1f7a3a; }"
        "QPushButton#depositButton:hover { background-color: #25994a; }"
        "QPushButton#withdrawButton { background-color: #8b1e3f; }"
        "QPushButton#withdrawButton:hover { background-color: #b32653; }"
        );
}

void WalletWindow::setupTransactionsModel()
{
    transactionsModel->setColumnCount(4);
    transactionsModel->setHeaderData(0, Qt::Horizontal, "Type");
    transactionsModel->setHeaderData(1, Qt::Horizontal, "Amount");
    transactionsModel->setHeaderData(2, Qt::Horizontal, "Timestamp");
    transactionsModel->setHeaderData(3, Qt::Horizontal, "Description");

    ui->transactionsTableView->setModel(transactionsModel);
    ui->transactionsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->transactionsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->transactionsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->transactionsTableView->horizontalHeader()->setStretchLastSection(true);
}

void WalletWindow::connectToServer()
{
    socket->abort();
    socket->connectToHost(QHostAddress(serverIp), serverPort);
    connectionTimer->start(CONNECTION_TIMEOUT);
}

void WalletWindow::requestWallet()
{
    pendingAction = PendingAction::GetWallet;
    connectToServer();
}

void WalletWindow::requestTransactions()
{
    pendingAction = PendingAction::GetTransactions;
    connectToServer();
}

void WalletWindow::sendDeposit(double amount)
{
    pendingAction = PendingAction::Deposit;
    pendingAmount = amount;
    connectToServer();
}

void WalletWindow::sendWithdraw(double amount)
{
    pendingAction = PendingAction::Withdraw;
    pendingAmount = amount;
    connectToServer();
}

void WalletWindow::updateBalanceDisplay()
{
    ui->balanceLineEdit->setText(QString::number(currentBalance, 'f', 2));
}

void WalletWindow::handleGetWalletResponse(const QJsonObject &obj)
{
    currentBalance = obj.value("balance").toDouble();
    updateBalanceDisplay();
    emit walletUpdated();
}

void WalletWindow::handleDepositResponse(const QJsonObject &obj)
{
    bool success = obj.value("success").toBool();
    QString message = obj.value("message").toString();
    if (success) {
        currentBalance = obj.value("new_balance").toDouble(currentBalance + pendingAmount);
        updateBalanceDisplay();
        QMessageBox::information(this, "Deposit", message.isEmpty() ? "Deposit successful" : message);
        emit walletUpdated();
        requestTransactions();
    } else {
        QMessageBox::critical(this, "Deposit", message.isEmpty() ? "Deposit failed" : message);
    }
}

void WalletWindow::handleWithdrawResponse(const QJsonObject &obj)
{
    bool success = obj.value("success").toBool();
    QString message = obj.value("message").toString();
    if (success) {
        currentBalance = obj.value("new_balance").toDouble(currentBalance - pendingAmount);
        updateBalanceDisplay();
        QMessageBox::information(this, "Withdraw", message.isEmpty() ? "Withdraw successful" : message);
        emit walletUpdated();
        requestTransactions();
    } else {
        QMessageBox::critical(this, "Withdraw", message.isEmpty() ? "Withdraw failed" : message);
    }
}

void WalletWindow::handleTransactionsResponse(const QJsonObject &obj)
{
    transactionsModel->removeRows(0, transactionsModel->rowCount());

    QJsonArray arr = obj.value("transactions").toArray();
    int row = 0;
    for (const auto &v : arr) {
        if (!v.isObject()) continue;
        QJsonObject t = v.toObject();

        QString type = t.value("type").toString();
        double amount = t.value("amount").toDouble();
        QString timestamp = t.value("timestamp").toString();
        QString description = t.value("description").toString();

        transactionsModel->insertRow(row);
        transactionsModel->setData(transactionsModel->index(row, 0), type);
        transactionsModel->setData(transactionsModel->index(row, 1), amount);
        transactionsModel->setData(transactionsModel->index(row, 2), timestamp);
        transactionsModel->setData(transactionsModel->index(row, 3), description);
        ++row;
    }
}

bool WalletWindow::validateAmountInput(QLineEdit *edit, double &amount, QString &error) const
{
    QString text = edit->text().trimmed();
    if (text.isEmpty()) {
        error = "Amount empty";
        return false;
    }

    bool ok = false;
    double val = text.toDouble(&ok);
    if (!ok || val <= 0) {
        error = "Invalid amount";
        return false;
    }

    amount = val;
    return true;
}

void WalletWindow::on_depositButton_clicked()
{
    double amount = 0.0;
    QString error;
    if (!validateAmountInput(ui->depositAmountLineEdit, amount, error)) {
        QMessageBox::warning(this, "Deposit", error);
        return;
    }

    if (currentUsername.isEmpty()) {
        QMessageBox::warning(this, "Deposit", "User not set");
        return;
    }

    sendDeposit(amount);
}

void WalletWindow::on_withdrawButton_clicked()
{
    double amount = 0.0;
    QString error;
    if (!validateAmountInput(ui->withdrawAmountLineEdit, amount, error)) {
        QMessageBox::warning(this, "Withdraw", error);
        return;
    }

    if (amount > currentBalance) {
        QMessageBox::warning(this, "Withdraw", "Insufficient balance");
        return;
    }

    if (currentUsername.isEmpty()) {
        QMessageBox::warning(this, "Withdraw", "User not set");
        return;
    }

    sendWithdraw(amount);
}

void WalletWindow::on_refreshButton_clicked()
{
    requestWallet();
    requestTransactions();
}

void WalletWindow::onConnected()
{
    QJsonObject obj;

    if (pendingAction == PendingAction::GetWallet) {
        obj["type"]     = "get_wallet";
        obj["username"] = currentUsername;
    } else if (pendingAction == PendingAction::Deposit) {
        obj["type"]     = "wallet_deposit";
        obj["username"] = currentUsername;
        obj["amount"]   = pendingAmount;
    } else if (pendingAction == PendingAction::Withdraw) {
        obj["type"]     = "wallet_withdraw";
        obj["username"] = currentUsername;
        obj["amount"]   = pendingAmount;
    } else if (pendingAction == PendingAction::GetTransactions) {
        obj["type"]     = "get_transactions";
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

void WalletWindow::onDisconnected()
{
}

void WalletWindow::onReadyRead()
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

        if (type == "get_wallet_response") {
            handleGetWalletResponse(obj);
        } else if (type == "wallet_deposit_response") {
            handleDepositResponse(obj);
        } else if (type == "wallet_withdraw_response") {
            handleWithdrawResponse(obj);
        } else if (type == "get_transactions_response") {
            handleTransactionsResponse(obj);
        }
    }

    pendingAction = PendingAction::None;
    pendingAmount = 0.0;
}

void WalletWindow::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    QString err = socket->errorString();
    QMessageBox::critical(this, "Network", err);
    emit networkError(err);
    pendingAction = PendingAction::None;
    pendingAmount = 0.0;
}
