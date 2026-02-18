#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainMenu; }
QT_END_NAMESPACE

enum class UserRole {
    NormalUser,
    Admin
};


struct MainMenuStats
{
    int totalAds            = 0;
    int userAds             = 0;
    int cartItems           = 0;
    int walletBalance       = 0;
    int pendingApprovalAds  = 0;
};

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();


    void setCurrentUser(const QString &username, bool isAdmin);
    void setCurrentUser(const QString &username, UserRole role);


    void setServerAddress(const QString &ip, quint16 port);

    void refreshDashboard();

signals:

    void logoutRequested();
    void openAddAdRequested();
    void openBrowseAdsRequested();
    void openCartRequested();
    void openWalletRequested();
    void openProfileRequested();
    void openAdminPanelRequested();

    void networkErrorOccurred(const QString &message);
    void statsUpdated(const MainMenuStats &stats);

private slots:
    // UI buttons
    void on_addAdButton_clicked();
    void on_browseAdsButton_clicked();
    void on_cartButton_clicked();
    void on_walletButton_clicked();
    void on_profileButton_clicked();
    void on_adminPanelButton_clicked();
    void on_logoutButton_clicked();

    // Network
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::MainMenu *ui;

    QString  currentUsername;
    UserRole currentRole;

    MainMenuStats stats;

    // Network
    QTcpSocket *socket;
    QTimer     *connectionTimer;
    QString     serverIp;
    quint16     serverPort;

    void setupUiDesign();
    void setupButtonIcons();
    void updateAdminVisibility();
    void updateStatsOnUi();

    // Network helpers
    void connectToServer();
    void sendInitialRequest();
    void handleInitResponse(const QJsonObject &obj);

    // Helpers
    bool isAdmin() const;
};

#endif // MAINMENU_H
