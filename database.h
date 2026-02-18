#ifndef DATABASE_H
#define DATABASE_H

#include "models.h"
#include <QMap>
#include <QList>
#include <QString>

class Database
{
public:
    static Database& instance();

    bool userExists(const QString &username) const;
    bool checkPassword(const QString &username, const QString &hash) const;
    void addUser(const User &user);
    User getUser(const QString &username) const;
    void updateUser(const User &user);

    int addAd(const Ad &ad);
    Ad getAd(int id) const;
    void updateAd(const Ad &ad);
    void updateAdStatus(int adId, const QString &status);
    QList<Ad> getAdsByStatus(const QString &status) const;
    QList<Ad> getUserAds(const QString &username) const;
    QList<Ad> getAllAds() const;

    void addToCart(const QString &username, int adId);
    QList<int> getCart(const QString &username) const;
    void removeFromCart(const QString &username, int adId);
    void clearCart(const QString &username);

    void addTransaction(const Transaction &t);
    QList<Transaction> getTransactions(const QString &username) const;

    void addPurchaseRecord(const PurchaseRecord &p);
    QList<PurchaseRecord> getPurchases(const QString &username) const;
    QList<PurchaseRecord> getSales(const QString &username) const;

    AdminStats getAdminStats() const;

    void saveToFile(const QString &path);
    void loadFromFile(const QString &path);

private:
    Database();

    QMap<QString, User> users;
    QMap<int, Ad> ads;
    QMap<QString, QList<int>> carts;
    QList<Transaction> transactions;
    QList<PurchaseRecord> purchases;

    int nextAdId;

    QString now() const;
};

#endif
