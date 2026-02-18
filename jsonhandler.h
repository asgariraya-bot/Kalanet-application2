#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

class JsonHandler
{
public:
    JsonHandler();

    QJsonObject handleRequest(const QJsonObject &req);

private:
    QJsonObject handleLogin(const QJsonObject &req);
    QJsonObject handleSignup(const QJsonObject &req);

    QJsonObject handleAddAd(const QJsonObject &req);
    QJsonObject handleGetAds(const QJsonObject &req);

    QJsonObject handleAddToCart(const QJsonObject &req);
    QJsonObject handleGetCart(const QJsonObject &req);
    QJsonObject handleRemoveFromCart(const QJsonObject &req);
    QJsonObject handlePurchaseCart(const QJsonObject &req);

    QJsonObject handleGetWallet(const QJsonObject &req);
    QJsonObject handleWalletDeposit(const QJsonObject &req);
    QJsonObject handleWalletWithdraw(const QJsonObject &req);
    QJsonObject handleGetTransactions(const QJsonObject &req);

    QJsonObject handleGetProfile(const QJsonObject &req);
    QJsonObject handleGetUserAds(const QJsonObject &req);
    QJsonObject handleGetUserPurchases(const QJsonObject &req);
    QJsonObject handleGetUserSales(const QJsonObject &req);

    QJsonObject handleGetPendingAds(const QJsonObject &req);
    QJsonObject handleGetApprovedAds(const QJsonObject &req);
    QJsonObject handleGetRejectedAds(const QJsonObject &req);
    QJsonObject handleApproveAd(const QJsonObject &req);
    QJsonObject handleRejectAd(const QJsonObject &req);
    QJsonObject handleGetAdminStats(const QJsonObject &req);

    QString hashPassword(const QString &plain) const;
    QString now() const;
};

#endif
