#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE


class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();


    void setServerAddress(const QString &ip, quint16 port);

signals:

    void loginSuccessful(const QString &username);

private slots:
    // UI buttons
    void on_loginButton_clicked();
    void on_exitButton_clicked();
    void on_refreshCaptchaButton_clicked();

    // Network
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::LoginWindow *ui;

    // Captcha
    QString currentCaptcha;
    void generateCaptcha();

    // Network
    QTcpSocket *socket;
    QTimer     *connectionTimer;

    QString serverIp;
    quint16 serverPort;

    void connectToServer();
    void sendLoginRequest(const QString &username, const QString &passwordHash);

    // Validation helpers
    bool validateUsername(const QString &u) const;
    bool validatePassword(const QString &p) const;
    bool validateCaptcha(const QString &c) const;


    void showValidationError(const QString &title, const QString &message) const;
    void showNetworkError(const QString &message) const;
    void showInfoMessage(const QString &title, const QString &message) const;
};

#endif
