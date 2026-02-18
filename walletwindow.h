#ifndef WALLETWINDOW_H
#define WALLETWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class WalletWindow; }
QT_END_NAMESPACE

class WalletWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WalletWindow(QWidget *parent = nullptr);
    ~WalletWindow();

    void setServerAddress(const QString &ip, quint16 port);
    void setCurrentUser(const QString &username);

signals:
    void walletUpdated();
    void networkError(const QString &msg);

private slots:
    void on_depositButton_clicked();
    void on_withdrawButton_clicked();
    void on_refreshButton_clicked();

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::WalletWindow *ui;

    QString currentUsername;

    QTcpSocket *socket;
    QTimer     *connectionTimer;
    QString     serverIp;
    quint16     serverPort;

    QStandardItemModel *transactionsModel;
    double currentBalance;

    enum class PendingAction {
        None,
        GetWallet,
        Deposit,
        Withdraw,
        GetTransactions
    };

    PendingAction pendingAction;
    double pendingAmount;

    void setupUiDesign();
    void setupTransactionsModel();
    void connectToServer();
    void requestWallet();
    void requestTransactions();
    void sendDeposit(double amount);
    void sendWithdraw(double amount);
    void updateBalanceDisplay();
    void handleGetWalletResponse(const QJsonObject &obj);
    void handleDepositResponse(const QJsonObject &obj);
    void handleWithdrawResponse(const QJsonObject &obj);
    void handleTransactionsResponse(const QJsonObject &obj);
    bool validateAmountInput(QLineEdit *edit, double &amount, QString &error) const;
};

#endif
