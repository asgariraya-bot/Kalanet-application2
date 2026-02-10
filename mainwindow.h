#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QObject>
#include "AuthService.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onUserLogin();
    void onAdminLogin();
    void onSignup();
    void onCreateAdClicked();
    void onViewAdsClicked();
    void onCartClicked();
    void onWalletClicked();
    void onUserProfileClicked();
    void onLogout();
    void onViewSystemStatsClicked();
    void onManageAdsClicked();
    void onServerSettingsClicked();
    void onAdminLogoutClicked();

private:
    Ui::MainWindow *ui;
    AuthService* authService;
};

#endif
