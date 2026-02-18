#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

namespace {
const QString SERVER_IP              = "127.0.0.1";
const quint16 SERVER_PORT            = 4545;
const int    CONNECTION_TIMEOUT_MS   = 5000;
const int    MIN_USERNAME_LENGTH     = 3;
const int    MIN_PASSWORD_LENGTH     = 8;
const int    CAPTCHA_LENGTH          = 5;
}



LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
{
    ui->setupUi(this);
    setWindowTitle("KalaNet - Login");


    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->usernameLineEdit->setPlaceholderText("Username");
    ui->passwordLineEdit->setPlaceholderText("Password (min 8 chars)");
    ui->captchaLineEdit->setPlaceholderText("Enter captcha");
    ui->usernameLineEdit->setFocus();


    generateCaptcha();


    connect(socket, &QTcpSocket::connected,    this, &LoginWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &LoginWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &LoginWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &LoginWindow::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        if (socket->state() == QAbstractSocket::ConnectingState ||
            socket->state() == QAbstractSocket::ConnectedState) {
            socket->abort();
        }
        QMessageBox::critical(this, "Timeout", "Server did not respond in time.");
    });
}

LoginWindow::~LoginWindow()
{
    delete ui;
}



void LoginWindow::generateCaptcha()
{
    const QString chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    QString cap;
    cap.reserve(CAPTCHA_LENGTH);

    for (int i = 0; i < CAPTCHA_LENGTH; ++i) {
        int idx = QRandomGenerator::global()->bounded(chars.size());
        cap.append(chars.at(idx));
    }

    currentCaptcha = cap;
    ui->captchaLabel->setText(cap);
}

void LoginWindow::on_refreshCaptchaButton_clicked()
{
    generateCaptcha();
    ui->captchaLineEdit->clear();
}



bool LoginWindow::validateUsername(const QString &u)
{
    if (u.isEmpty() || u.length() < MIN_USERNAME_LENGTH)
        return false;

    QRegularExpression re("^[A-Za-z0-9_]+$");
    return re.match(u).hasMatch();
}

bool LoginWindow::validatePassword(const QString &p)
{
    if (p.length() < MIN_PASSWORD_LENGTH)
        return false;

    return true;
}

bool LoginWindow::validateCaptcha(const QString &c)
{
    return !c.isEmpty() &&
           c.compare(currentCaptcha, Qt::CaseInsensitive) == 0;
}



void LoginWindow::connectToServer()
{
    if (socket->state() == QAbstractSocket::ConnectedState ||
        socket->state() == QAbstractSocket::ConnectingState) {
        socket->abort();
    }

    socket->connectToHost(QHostAddress(SERVER_IP), SERVER_PORT);
    connectionTimer->start(CONNECTION_TIMEOUT_MS);
}

void LoginWindow::sendLoginRequest(const QString &username, const QString &passwordHash)
{

    QJsonObject obj;
    obj["type"]          = "login";
    obj["username"]      = username;
    obj["password_hash"] = passwordHash;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    socket->flush();
}



void LoginWindow::on_loginButton_clicked()
{
    const QString username = ui->usernameLineEdit->text().trimmed();
    const QString password = ui->passwordLineEdit->text();
    const QString captcha  = ui->captchaLineEdit->text().trimmed();


    if (username.isEmpty() || password.isEmpty() || captcha.isEmpty()) {
        QMessageBox::warning(this, "Invalid input", "All fields must be filled.");
        return;
    }

    if (!validateUsername(username)) {
        QMessageBox::warning(this, "Invalid username",
                             QString("Username must be at least %1 characters and contain only letters, digits, or underscore.")
                                 .arg(MIN_USERNAME_LENGTH));
        return;
    }

    if (!validatePassword(password)) {
        QMessageBox::warning(this, "Invalid password",
                             QString("Password must be at least %1 characters.").arg(MIN_PASSWORD_LENGTH));
        return;
    }


    if (!validateCaptcha(captcha)) {
        QMessageBox::warning(this, "Invalid captcha", "Captcha is incorrect.");
        generateCaptcha();
        ui->captchaLineEdit->clear();
        return;
    }


    QByteArray hashBytes = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString   hashHex    = QString::fromUtf8(hashBytes.toHex());


    connectToServer();

    sendLoginRequest(username, hashHex);
}

void LoginWindow::on_exitButton_clicked()
{
    close();
}


void LoginWindow::onConnected()
{

}

void LoginWindow::onDisconnected()
{

}

void LoginWindow::onReadyRead()
{

    QByteArray allData = socket->readAll();
    QList<QByteArray> messages = allData.split('\n');

    for (QByteArray msg : messages) {
        msg = msg.trimmed();
        if (msg.isEmpty())
            continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(msg, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject())
            continue;

        QJsonObject obj = doc.object();
        const QString type = obj.value("type").toString();

        if (type == "login_response") {
            bool success       = obj.value("success").toBool(false);
            QString message    = obj.value("message").toString();
            QString username   = ui->usernameLineEdit->text().trimmed();

            if (success) {
                if (message.isEmpty())
                    message = "Login successful!";

                QMessageBox::information(this, "Login", message);


                emit loginSuccessful(username);


                close();
            } else {
                if (message.isEmpty())
                    message = "Invalid username or password.";

                QMessageBox::critical(this, "Login failed", message);
                generateCaptcha();
                ui->captchaLineEdit->clear();
            }
        }
    }
}

void LoginWindow::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    QMessageBox::critical(this, "Network error", socket->errorString());
}
