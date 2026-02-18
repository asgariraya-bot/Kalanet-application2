#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class ProfileWindow; }
QT_END_NAMESPACE

class ProfileWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfileWindow(QWidget *parent = nullptr);
    ~ProfileWindow();

    void setServerAddress(const QString &ip, quint16 port);
    void setCurrentUser(const QString &username);

signals:
    void networkError(const QString &msg);

private slots:
    void on_refreshButton_clicked();

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::ProfileWindow *ui;

    QString currentUsername;

    QTcpSocket *socket;
    QTimer     *connectionTimer;
    QString     serverIp;
    quint16     serverPort;

    QStandardItemModel *userAdsModel;
    QStandardItemModel *purchasesModel;
    QStandardItemModel *salesModel;

    enum class PendingRequest {
        None,
        GetProfile,
        GetUserAds,
        GetPurchases,
        GetSales
    };

    PendingRequest pendingRequest;

    void setupUiDesign();
    void setupModels();
    void connectToServer();
    void requestProfile();
    void requestUserAds();
    void requestPurchases();
    void requestSales();

    void handleProfileResponse(const QJsonObject &obj);
    void handleUserAdsResponse(const QJsonObject &obj);
    void handlePurchasesResponse(const QJsonObject &obj);
    void handleSalesResponse(const QJsonObject &obj);

    void sendRequestForCurrentPending();
};

#endif
