#include "signupwindow.h"
#include "ui_signupwindow.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

namespace {
const QString DEFAULT_SERVER_IP        = "127.0.0.1";
const quint16 DEFAULT_SERVER_PORT      = 4545;
const int     CONNECTION_TIMEOUT_MS    = 5000;
const int     MIN_NAME_LENGTH          = 2;
const int     MIN_USERNAME_LENGTH      = 3;
const int     MIN_PASSWORD_LENGTH      = 8;
}


SignUpWindow::SignUpWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SignUpWindow)
    , socket(new QTcpSocket(this))
    , connectionTimer(new QTimer(this))
    , serverIp(DEFAULT_SERVER_IP)
    , serverPort(DEFAULT_SERVER_PORT)
{
    ui->setupUi(this);
    setWindowTitle("KalaNet - Sign Up");

    // UI placeholders
    ui->nameLineEdit->setPlaceholderText("Full name");
    ui->usernameLineEdit->setPlaceholderText("Username");
    ui->emailLineEdit->setPlaceholderText("Email");
    ui->phoneLineEdit->setPlaceholderText("Phone number");
    ui->passwordLineEdit->setPlaceholderText("Password (min 8 chars)");
    ui->confirmPasswordLineEdit->setPlaceholderText("Repeat password");

    // Mask passwords
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);

    ui->nameLineEdit->setFocus();

    // Network connections
    connect(socket, &QTcpSocket::connected,    this, &SignUpWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &SignUpWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &SignUpWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,this, &SignUpWindow::onSocketError);

    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, [this]() {
        socket->abort();
        showNetworkError("Server did not respond in time.");
    });
}

SignUpWindow::~SignUpWindow()
{
    delete ui;
}



void SignUpWindow::setServerAddress(const QString &ip, quint16 port)
{
    serverIp   = ip;
    serverPort = port;
}



bool SignUpWindow::validateName(const QString &name) const
{
    return name.trimmed().length() >= MIN_NAME_LENGTH;
}

bool SignUpWindow::validateUsername(const QString &username) const
{
    if (username.length() < MIN_USERNAME_LENGTH)
        return false;

    QRegularExpression re("^[A-Za-z0-9_]+$");
    return re.match(username).hasMatch();
}

bool SignUpWindow::validateEmail(const QString &email) const
{
    QRegularExpression re(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    return re.match(email).hasMatch();
}

bool SignUpWindow::validatePhone(const QString &phone) const
{
    QRegularExpression re(R"(^\+?[0-9]{7,15}$)");
    return re.match(phone).hasMatch();
}

bool SignUpWindow::validatePassword(const QString &password) const
{
    return password.length() >= MIN_PASSWORD_LENGTH;
}

bool SignUpWindow::validatePasswordMatch(const QString &p1, const QString &p2) const
{
    return p1 == p2;
}



void SignUpWindow::clearInputs()
{
    ui->nameLineEdit->clear();
    ui->usernameLineEdit->clear();
    ui->emailLineEdit->clear();
    ui->phoneLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->confirmPasswordLineEdit->clear();
}

void SignUpWindow::showValidationError(const QString &title, const QString &message) const
{
    QMessageBox::warning(const_cast<SignUpWindow*>(this), title, message);
}

void SignUpWindow::showNetworkError(const QString &message) const
{
    QMessageBox::critical(const_cast<SignUpWindow*>(this), "Network error", message);
}

void SignUpWindow::showInfoMessage(const QString &title, const QString &message) const
{
    QMessageBox::information(const_cast<SignUpWindow*>(this), title, message);
}



void SignUpWindow::connectToServer()
{
    socket->abort();
    socket->connectToHost(QHostAddress(serverIp), serverPort);
    connectionTimer->start(CONNECTION_TIMEOUT_MS);
}

void SignUpWindow::sendSignupRequest(const QMap<QString, QString> &data)
{
    QJsonObject obj;
    obj["type"]          = "signup";
    obj["name"]          = data["name"];
    obj["username"]      = data["username"];
    obj["email"]         = data["email"];
    obj["phone"]         = data["phone"];
    obj["password_hash"] = data["password_hash"];

    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact);
    json.append('\n');

    socket->write(json);
    socket->flush();
}



void SignUpWindow::on_signUpButton_clicked()
{
    QString name     = ui->nameLineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString email    = ui->emailLineEdit->text().trimmed();
    QString phone    = ui->phoneLineEdit->text().trimmed();
    QString pass1    = ui->passwordLineEdit->text();
    QString pass2    = ui->confirmPasswordLineEdit->text();

    // Validation
    if (!validateName(name)) {
        showValidationError("Invalid name", "Name is too short.");
        return;
    }
    if (!validateUsername(username)) {
        showValidationError("Invalid username", "Username must be at least 3 characters and contain only letters, digits, or underscore.");
        return;
    }
    if (!validateEmail(email)) {
        showValidationError("Invalid email", "Please enter a valid email address.");
        return;
    }
    if (!validatePhone(phone)) {
        showValidationError("Invalid phone", "Please enter a valid phone number.");
        return;
    }
    if (!validatePassword(pass1)) {
        showValidationError("Invalid password", "Password must be at least 8 characters.");
        return;
    }
    if (!validatePasswordMatch(pass1, pass2)) {
        showValidationError("Password mismatch", "Passwords do not match.");
        return;
    }

    // Hash password
    QByteArray hashBytes = QCryptographicHash::hash(pass1.toUtf8(), QCryptographicHash::Sha256);
    QString hashHex = QString::fromUtf8(hashBytes.toHex());

    // Prepare data
    QMap<QString, QString> data;
    data["name"]          = name;
    data["username"]      = username;
    data["email"]         = email;
    data["phone"]         = phone;
    data["password_hash"] = hashHex;

    // Send
    connectToServer();
    sendSignupRequest(data);
}

void SignUpWindow::on_backButton_clicked()
{
    emit backToLoginRequested();
    close();
}



void SignUpWindow::onConnected()
{
    // Optional: update UI
}

void SignUpWindow::onDisconnected()
{
    // Optional: update UI
}

void SignUpWindow::onReadyRead()
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
        QString type = obj["type"].toString();

        if (type == "signup_response") {
            bool success = obj["success"].toBool();
            QString message = obj["message"].toString();

            if (success) {
                showInfoMessage("Success", message.isEmpty() ? "Sign up successful!" : message);
                emit signupSuccessful(ui->usernameLineEdit->text().trimmed());
                close();
            } else {
                showValidationError("Sign up failed", message.isEmpty() ? "Username or email already exists." : message);
            }
        }
    }
}

void SignUpWindow::onSocketError(QAbstractSocket::SocketError)
{
    connectionTimer->stop();
    showNetworkError(socket->errorString());
}
