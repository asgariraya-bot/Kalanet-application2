#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    authService = new AuthService;
        ui->stackedWidget->setStyleSheet(
        "QWidget { background-color: #0d1b2a; }"
        "QPushButton { background-color: #1b263b; color: white; font-size: 16px; padding: 8px; border-radius: 6px; }"
        "QPushButton:hover { background-color: #415a77; }"
        "QLineEdit { background-color: #1b263b; color: white; border: 1px solid #415a77; border-radius: 4px; padding: 4px; font-size: 14px; }"
        );

    ui->page->setStyleSheet("background-color: #0d1b2a;");
    ui->page_3->setStyleSheet("background-color: #1b263b;");
    ui->page_7->setStyleSheet("background-color: #1b263b;");
    ui->page_2->setStyleSheet("background-color: #0d1b2a;");
    ui->page_4->setStyleSheet("background-color: #0d1b2a;");

    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->adminPasswordLineEdit->setEchoMode(QLineEdit::Password);

    connect(ui->userModeButton, &QPushButton::clicked, [=](){ ui->stackedWidget->setCurrentWidget(ui->page_7); });
    connect(ui->adminModeButton, &QPushButton::clicked, [=](){ ui->stackedWidget->setCurrentWidget(ui->page_3); });

    connect(ui->userLoginButton, &QPushButton::clicked, this, &MainWindow::onUserLogin);
    connect(ui->adminLoginButton, &QPushButton::clicked, this, &MainWindow::onAdminLogin);
    connect(ui->signupButton, &QPushButton::clicked, this, &MainWindow::onSignup);

    connect(ui->btnCreateAd, &QPushButton::clicked, this, &MainWindow::onCreateAdClicked);
    connect(ui->btnViewAd, &QPushButton::clicked, this, &MainWindow::onViewAdsClicked);
    connect(ui->btnCart, &QPushButton::clicked, this, &MainWindow::onCartClicked);
    connect(ui->btnWallet, &QPushButton::clicked, this, &MainWindow::onWalletClicked);
    connect(ui->btnUserProfile, &QPushButton::clicked, this, &MainWindow::onUserProfileClicked);
    connect(ui->btnUserLogout, &QPushButton::clicked, this, &MainWindow::onLogout);

    connect(ui->btnViewSystemStats, &QPushButton::clicked, this, &MainWindow::onViewSystemStatsClicked);
    connect(ui->btManageAds, &QPushButton::clicked, this, &MainWindow::onManageAdsClicked);
    connect(ui->btnServerSetings, &QPushButton::clicked, this, &MainWindow::onServerSettingsClicked);
    connect(ui->btnAdminLogout, &QPushButton::clicked, this, &MainWindow::onAdminLogoutClicked);

    ui->stackedWidget->setCurrentWidget(ui->page);
}

MainWindow::~MainWindow()
{
    delete authService;
    delete ui;
}

void MainWindow::onUserLogin() {
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if(authService->loginUser(username.toStdString(), password.toStdString())) {
        ui->stackedWidget->setCurrentWidget(ui->page_2);
    } else {
        QMessageBox::warning(this, "Error", "نام کاربری یا رمز عبور اشتباه است");
    }
}

void MainWindow::onAdminLogin() {
    QString pass = ui->adminPasswordLineEdit->text();

    if(authService->loginAdmin(pass.toStdString())) {
        ui->stackedWidget->setCurrentWidget(ui->page_4);
    } else {
        QMessageBox::warning(this, "Error", "رمز عبور ادمین اشتباه است");
    }
}

void MainWindow::onSignup() {
    QString name = ui->nameLineEdit->text();
    QString username = ui->usernameLineEdit->text();
    QString email = ui->emailLineEdit->text();
    QString phone = ui->phoneLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    if(!authService->validateEmail(email.toStdString())) {
        QMessageBox::warning(this, "Error", "ایمیل معتبر نیست باید شامل @ و gmail.com باشد ");
        return;
    }
    if(!authService->validatePhone(phone.toStdString())) {
        QMessageBox::warning(this, "Error", " شماره تلفن معتبر نیست باید 11 رقم باشد");
        return;
    }
    if(!authService->validatePassword(password.toStdString())) {
        QMessageBox::warning(this, "Error", "رمز عبور باید 8عبارت وشامل 2 حرف باشد ");
        return;
    }

    authService->signupUser(name.toStdString(), username.toStdString(),
                            email.toStdString(), phone.toStdString(),
                            password.toStdString());
    QMessageBox::information(this, "Success", "ثبت‌نام با موفقیت انجام شد");
}

void MainWindow::onCreateAdClicked() { /* TODO: implement */ }
void MainWindow::onViewAdsClicked() { /* TODO: implement */ }
void MainWindow::onCartClicked() { /* TODO: implement */ }
void MainWindow::onWalletClicked() { /* TODO: implement */ }
void MainWindow::onUserProfileClicked() { /* TODO: implement */ }
void MainWindow::onLogout() {
    ui->stackedWidget->setCurrentWidget(ui->page);
}
void MainWindow::onViewSystemStatsClicked() { /* TODO: implement */ }
void MainWindow::onManageAdsClicked() { /* TODO: implement */ }
void MainWindow::onServerSettingsClicked() { /* TODO: implement */ }
void MainWindow::onAdminLogoutClicked() {
    ui->stackedWidget->setCurrentWidget(ui->page); // back to role select
}
