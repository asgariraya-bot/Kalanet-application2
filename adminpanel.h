#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class AdminPanel; }
QT_END_NAMESPACE

class AdminPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminPanel(QWidget *parent = nullptr);
    ~AdminPanel();

    void setServerAddress(const QString &ip, quint16 port);
    void setCurrentAdmin(const QString &username);

signals:
    void networkError(const QString &msg);

private slots:
    void on_refreshButton_clicked();
    void on_approveButton_clicked();
    void on_rejectButton_clicked();
    void on_tabWidget_currentChanged(int index);

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::AdminPanel *ui;

    QString adminUsername;

    QTcpSocket *socket;
    QTimer     *connectionTimer;
    QString     serverIp;
    quint16     serverPort;

    QStandardItemModel *pendingModel;
    QStandardItemModel *approvedModel;
    QStandardItemModel *rejectedModel;

    enum class PendingRequest {
        None,
        GetPending,
        GetApproved,
        GetRejected,
        ApproveAd,
        RejectAd,
        GetStats
    };

    PendingRequest pendingRequest;
    int selectedAdId;

    void setupUiDesign();
    void setupModels();
    void connectToServer();
    void requestPendingAds();
    void requestApprovedAds();
    void requestRejectedAds();
    void requestStats();
    void sendApproveRequest(int adId);
    void sendRejectRequest(int adId);

    void handlePendingResponse(const QJsonObject &obj);
    void handleApprovedResponse(const QJsonObject &obj);
    void handleRejectedResponse(const QJsonObject &obj);
    void handleApproveResponse(const QJsonObject &obj);
    void handleRejectResponse(const QJsonObject &obj);
    void handleStatsResponse(const QJsonObject &obj);

    void sendRequestForCurrentPending();
};

#endif
