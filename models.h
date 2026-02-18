#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QList>
#include <QDateTime>

struct User {
    QString username;
    QString passwordHash;
    QString name;
    QString email;
    QString phone;
    QString joinDate;
    double walletBalance;
    int adsCount;
    int purchasesCount;
    int salesCount;
    bool isAdmin;
};

struct Ad {
    int id;
    QString owner;
    QString title;
    QString description;
    double price;
    QString category;
    QString status;
    QString imageBase64;
    QString createdAt;
    QString updatedAt;
};

struct Transaction {
    QString username;
    QString type;
    double amount;
    QString timestamp;
    QString description;
    QString relatedAdTitle;
    int relatedAdId;
};

struct PurchaseRecord {
    QString buyer;
    QString seller;
    QString title;
    double price;
    QString date;
    int adId;
};

struct AdminStats {
    int totalUsers;
    int totalAds;
    int pendingAds;
    int approvedAds;
    int rejectedAds;
    int totalTransactions;
    int totalPurchases;
};

#endif
