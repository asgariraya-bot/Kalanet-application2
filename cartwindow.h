#ifndef CARTWINDOW_H
#define CARTWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class CartWindow; }
QT_END_NAMESPACE

class CartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CartWindow(QWidget *parent = nullptr);
    ~CartWindow();

    void setServerAddress(const QString &ip, quint16 port);
    void setCurrentUser(const QString &username);

signals:
    void cartUpdated();
    void purchaseCompleted();
    void networkError(const QString &msg);

private slots:
    void on_refreshButton_clicked();
    void on_removeButton_clicked();
    void on_purchaseButton_clicked();

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::CartWindow *ui;

    QString currentUsername;

    QTcpSocket *socket;
    QTimer     *connectionTimer;
    QString     serverIp;
    quint16     serverPort;

    QStandardItemModel *model;
    double totalPrice;

    struct CartItem {
        int     id;
        QString title;
        QString category;
        double  price;
    };

    QList<CartItem> items;

    void setupUiDesign();
    void setupModel();
    void connectToServer();
    void requestCart();
    void sendRemoveRequest(int adId);
    void sendPurchaseRequest();
    void updateTotalPrice();
    void populateTable(const QList<CartItem> &list);
};

#endif
