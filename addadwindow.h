#ifndef ADDADWINDOW_H
#define ADDADWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class AddAdWindow; }
QT_END_NAMESPACE

class AddAdWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddAdWindow(QWidget *parent = nullptr);
    ~AddAdWindow();

    void setServerAddress(const QString &ip, quint16 port);
    void setCurrentUser(const QString &username);

signals:
    void adCreated();
    void networkError(const QString &msg);

private slots:
    void on_browseImageButton_clicked();
    void on_submitButton_clicked();
    void on_cancelButton_clicked();

    // Network
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::AddAdWindow *ui;

    QString currentUsername;
    QString selectedImagePath;

    QTcpSocket *socket;
    QTimer     *connectionTimer;
    QString     serverIp;
    quint16     serverPort;

    void setupUiDesign();
    bool validateInputs(QString &errorMessage) const;
    void connectToServer();
    void sendAddAdRequest();

    QByteArray loadImageAsBase64() const;
};

#endif
