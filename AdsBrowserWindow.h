#ifndef ADSBROWSERWINDOW_H
#define ADSBROWSERWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class AdsBrowserWindow; }
QT_END_NAMESPACE

class AdsBrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdsBrowserWindow(QWidget *parent = nullptr);
    ~AdsBrowserWindow();

    void setServerAddress(const QString &ip, quint16 port);
    void setCurrentUser(const QString &username);

signals:
    void addToCartRequested(int adId);
    void networkError(const QString &msg);

private slots:
    void on_searchLineEdit_textChanged(const QString &text);
    void on_categoryFilterComboBox_currentIndexChanged(int index);
    void on_minPriceSpinBox_valueChanged(double value);
    void on_maxPriceSpinBox_valueChanged(double value);
    void on_refreshButton_clicked();
    void on_addToCartButton_clicked();

    // Network
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::AdsBrowserWindow *ui;

    QString currentUsername;

    QTcpSocket *socket;
    QTimer     *connectionTimer;
    QString     serverIp;
    quint16     serverPort;

    QStandardItemModel *model;

    struct AdItem {
        int     id;
        QString title;
        QString category;
        double  price;
        QString status;
        QString thumbnailBase64;
    };

    QList<AdItem> allAds;

    void setupUiDesign();
    void setupModel();
    void connectToServer();
    void requestAdsList();
    void applyFilters();
    void populateTable(const QList<AdItem> &ads);

    QList<AdItem> filteredAds() const;
};

#endif // ADSBROWSERWINDOW_H
