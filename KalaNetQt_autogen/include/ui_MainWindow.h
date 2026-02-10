/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *verticalLayout;
    QPushButton *userModeButton;
    QPushButton *adminModeButton;
    QWidget *page_3;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *adminPasswordLineEdit;
    QPushButton *adminLoginButton;
    QWidget *page_7;
    QVBoxLayout *verticalLayout_5;
    QLineEdit *nameLineEdit;
    QLineEdit *usernameLineEdit;
    QLineEdit *emailLineEdit;
    QLineEdit *phoneLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *signupButton;
    QPushButton *userLoginButton;
    QWidget *page_2;
    QVBoxLayout *verticalLayout_4;
    QPushButton *btnCreateAd;
    QPushButton *btnViewAd;
    QPushButton *btnCart;
    QPushButton *btnWallet;
    QPushButton *btnUserProfile;
    QPushButton *btnUserLogout;
    QWidget *page_4;
    QVBoxLayout *verticalLayout_2;
    QPushButton *btnViewSystemStats;
    QPushButton *btManageAds;
    QPushButton *btnServerSetings;
    QPushButton *btnAdminLogout;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(60, 10, 321, 291));
        stackedWidget->setMinimumSize(QSize(321, 0));
        page = new QWidget();
        page->setObjectName("page");
        verticalLayout = new QVBoxLayout(page);
        verticalLayout->setObjectName("verticalLayout");
        userModeButton = new QPushButton(page);
        userModeButton->setObjectName("userModeButton");

        verticalLayout->addWidget(userModeButton);

        adminModeButton = new QPushButton(page);
        adminModeButton->setObjectName("adminModeButton");

        verticalLayout->addWidget(adminModeButton);

        stackedWidget->addWidget(page);
        page_3 = new QWidget();
        page_3->setObjectName("page_3");
        verticalLayout_3 = new QVBoxLayout(page_3);
        verticalLayout_3->setObjectName("verticalLayout_3");
        adminPasswordLineEdit = new QLineEdit(page_3);
        adminPasswordLineEdit->setObjectName("adminPasswordLineEdit");

        verticalLayout_3->addWidget(adminPasswordLineEdit);

        adminLoginButton = new QPushButton(page_3);
        adminLoginButton->setObjectName("adminLoginButton");

        verticalLayout_3->addWidget(adminLoginButton);

        stackedWidget->addWidget(page_3);
        page_7 = new QWidget();
        page_7->setObjectName("page_7");
        verticalLayout_5 = new QVBoxLayout(page_7);
        verticalLayout_5->setObjectName("verticalLayout_5");
        nameLineEdit = new QLineEdit(page_7);
        nameLineEdit->setObjectName("nameLineEdit");
        nameLineEdit->setMinimumSize(QSize(299, 0));

        verticalLayout_5->addWidget(nameLineEdit);

        usernameLineEdit = new QLineEdit(page_7);
        usernameLineEdit->setObjectName("usernameLineEdit");
        usernameLineEdit->setMinimumSize(QSize(299, 0));

        verticalLayout_5->addWidget(usernameLineEdit);

        emailLineEdit = new QLineEdit(page_7);
        emailLineEdit->setObjectName("emailLineEdit");
        emailLineEdit->setMinimumSize(QSize(299, 0));

        verticalLayout_5->addWidget(emailLineEdit);

        phoneLineEdit = new QLineEdit(page_7);
        phoneLineEdit->setObjectName("phoneLineEdit");
        phoneLineEdit->setMinimumSize(QSize(299, 0));

        verticalLayout_5->addWidget(phoneLineEdit);

        passwordLineEdit = new QLineEdit(page_7);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setMinimumSize(QSize(299, 0));

        verticalLayout_5->addWidget(passwordLineEdit);

        signupButton = new QPushButton(page_7);
        signupButton->setObjectName("signupButton");
        signupButton->setMinimumSize(QSize(299, 0));

        verticalLayout_5->addWidget(signupButton);

        userLoginButton = new QPushButton(page_7);
        userLoginButton->setObjectName("userLoginButton");
        userLoginButton->setMinimumSize(QSize(299, 0));

        verticalLayout_5->addWidget(userLoginButton);

        stackedWidget->addWidget(page_7);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        verticalLayout_4 = new QVBoxLayout(page_2);
        verticalLayout_4->setObjectName("verticalLayout_4");
        btnCreateAd = new QPushButton(page_2);
        btnCreateAd->setObjectName("btnCreateAd");

        verticalLayout_4->addWidget(btnCreateAd);

        btnViewAd = new QPushButton(page_2);
        btnViewAd->setObjectName("btnViewAd");

        verticalLayout_4->addWidget(btnViewAd);

        btnCart = new QPushButton(page_2);
        btnCart->setObjectName("btnCart");

        verticalLayout_4->addWidget(btnCart);

        btnWallet = new QPushButton(page_2);
        btnWallet->setObjectName("btnWallet");

        verticalLayout_4->addWidget(btnWallet);

        btnUserProfile = new QPushButton(page_2);
        btnUserProfile->setObjectName("btnUserProfile");

        verticalLayout_4->addWidget(btnUserProfile);

        btnUserLogout = new QPushButton(page_2);
        btnUserLogout->setObjectName("btnUserLogout");

        verticalLayout_4->addWidget(btnUserLogout);

        stackedWidget->addWidget(page_2);
        page_4 = new QWidget();
        page_4->setObjectName("page_4");
        verticalLayout_2 = new QVBoxLayout(page_4);
        verticalLayout_2->setObjectName("verticalLayout_2");
        btnViewSystemStats = new QPushButton(page_4);
        btnViewSystemStats->setObjectName("btnViewSystemStats");

        verticalLayout_2->addWidget(btnViewSystemStats);

        btManageAds = new QPushButton(page_4);
        btManageAds->setObjectName("btManageAds");

        verticalLayout_2->addWidget(btManageAds);

        btnServerSetings = new QPushButton(page_4);
        btnServerSetings->setObjectName("btnServerSetings");

        verticalLayout_2->addWidget(btnServerSetings);

        btnAdminLogout = new QPushButton(page_4);
        btnAdminLogout->setObjectName("btnAdminLogout");

        verticalLayout_2->addWidget(btnAdminLogout);

        stackedWidget->addWidget(page_4);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        MainWindow->setProperty("ss", QVariant(QString()));
        userModeButton->setText(QCoreApplication::translate("MainWindow", "\331\210\330\261\331\210\330\257 \332\251\330\247\330\261\330\250\330\261", nullptr));
        adminModeButton->setText(QCoreApplication::translate("MainWindow", "\331\210\330\261\331\210\330\257 \331\205\330\257\333\214\330\261", nullptr));
        adminPasswordLineEdit->setText(QCoreApplication::translate("MainWindow", "\330\261\331\205\330\262 \331\205\330\257\333\214\330\261:", nullptr));
        adminLoginButton->setText(QCoreApplication::translate("MainWindow", "\331\210\330\261\331\210\330\257", nullptr));
        nameLineEdit->setText(QCoreApplication::translate("MainWindow", "\331\206\330\247\331\205 \331\210 \331\206\330\247\331\205 \330\256\330\247\331\206\331\210\330\247\330\257\332\257\333\214:", nullptr));
        usernameLineEdit->setText(QCoreApplication::translate("MainWindow", "\331\206\330\247\331\205 \332\251\330\247\330\261\330\250\330\261\333\214:", nullptr));
        emailLineEdit->setText(QCoreApplication::translate("MainWindow", "\330\242\330\257\330\261\330\263 \330\247\333\214\331\205\333\214\331\204:", nullptr));
        phoneLineEdit->setText(QCoreApplication::translate("MainWindow", "\330\264\331\205\330\247\330\261\331\207 \333\214 \331\207\331\205\330\261\330\247\331\207:", nullptr));
        passwordLineEdit->setText(QCoreApplication::translate("MainWindow", "\330\261\331\205\330\262 \330\271\330\250\331\210\330\261:", nullptr));
        signupButton->setText(QCoreApplication::translate("MainWindow", "\330\253\330\250\330\252 \331\206\330\247\331\205", nullptr));
        userLoginButton->setText(QCoreApplication::translate("MainWindow", "\331\210\330\261\331\210\330\257", nullptr));
        btnCreateAd->setText(QCoreApplication::translate("MainWindow", "\330\253\330\250\330\252 \330\242\332\257\331\207\333\214", nullptr));
        btnViewAd->setText(QCoreApplication::translate("MainWindow", "\331\205\330\264\330\247\331\207\330\257\331\207 \333\214 \330\242\332\257\331\207\333\214 ", nullptr));
        btnCart->setText(QCoreApplication::translate("MainWindow", "\330\263\330\250\330\257 \330\256\330\261\333\214\330\257", nullptr));
        btnWallet->setText(QCoreApplication::translate("MainWindow", "\332\251\333\214\331\201 \331\276\331\210\331\204", nullptr));
        btnUserProfile->setText(QCoreApplication::translate("MainWindow", "\331\276\330\261\331\210\331\201\330\247\333\214\331\204 \332\251\330\247\330\261\330\250\330\261\333\214", nullptr));
        btnUserLogout->setText(QCoreApplication::translate("MainWindow", "\330\256\330\261\331\210\330\254", nullptr));
        btnViewSystemStats->setText(QCoreApplication::translate("MainWindow", "\330\242\331\205\330\247\330\261 \332\251\331\204\333\214 \330\263\330\247\331\205\330\247\331\206\331\207", nullptr));
        btManageAds->setText(QCoreApplication::translate("MainWindow", "\330\252\330\247\333\214\333\214\330\257/\330\261\330\257 \330\242\332\257\331\207\333\214 \331\207\330\247", nullptr));
        btnServerSetings->setText(QCoreApplication::translate("MainWindow", "\331\205\330\257\333\214\330\261\333\214\330\252 \330\252\331\206\330\270\333\214\331\205\330\247\330\252 ", nullptr));
        btnAdminLogout->setText(QCoreApplication::translate("MainWindow", "\330\256\330\261\331\210\330\254", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
