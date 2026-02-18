#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class SignUpWindow; }
QT_END_NAMESPACE

class SignUpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignUpWindow(QWidget *parent = nullptr);
    ~SignUpWindow();


    void setServerAddress(const QString &ip, quint16 port);

signals:
    void signupSuccessful(const QString &username);
    void backToLoginRequested();

private slots:
    // UI
    void on_signUpButton_clicked();
    void on_backButton_clicked();

    // Network
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::SignUpWindow *ui;

    // Network
    QTcpSocket *socket;
    QTimer     *connectionTimer;
    QString     serverIp;
    quint16     serverPort;

    // Validation helpers
    bool validateName(const QString &name) const;
    bool validateUsername(const QString &username) const;
    bool validateEmail(const QString &email) const;
    bool validatePhone(const QString &phone) const;
    bool validatePassword(const QString &password) const;
    bool validatePasswordMatch(const QString &p1, const QString &p2) const;

    // Network helpers
    void connectToServer();
    void sendSignupRequest(const QMap<QString, QString> &data);

    // UI helpers
    void clearInputs();
    void showValidationError(const QString &title, const QString &message) const;
    void showNetworkError(const QString &message) const;
    void showInfoMessage(const QString &title, const QString &message) const;
};

#endif
