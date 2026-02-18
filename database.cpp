#include "database.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDateTime>

Database::Database()
    : nextAdId(1)
{
}

Database& Database::instance()
{
    static Database db;
    return db;
}

QString Database::now() const
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

bool Database::userExists(const QString &username) const
{
    return users.contains(username);
}

bool Database::checkPassword(const QString &username, const QString &hash) const
{
    if (!users.contains(username)) return false;
    return users[username].passwordHash == hash;
}

void Database::addUser(const User &user)
{
    users[user.username] = user;
}

User Database::getUser(const QString &username) const
{
    return users.value(username);
}

void Database::updateUser(const User &user)
{
    users[user.username] = user;
}

int Database::addAd(const Ad &ad)
{
    Ad a = ad;
    a.id = nextAdId++;
    a.createdAt = now();
    a.updatedAt = a.createdAt;
    ads[a.id] = a;
    return a.id;
}

Ad Database::getAd(int id) const
{
    return ads.value(id);
}

void Database::updateAd(const Ad &ad)
{
    Ad a = ad;
    a.updatedAt = now();
    ads[a.id] = a;
}

void Database::updateAdStatus(int adId, const QString &status)
{
    if (ads.contains(adId)) {
        ads[adId].status = status;
        ads[adId].updatedAt = now();
    }
}

QList<Ad> Database::getAdsByStatus(const QString &status) const
{
    QList<Ad> list;
    for (const auto &a : ads.values())
        if (a.status == status)
            list.append(a);
    return list;
}

QList<Ad> Database::getUserAds(const QString &username) const
{
    QList<Ad> list;
    for (const auto &a : ads.values())
        if (a.owner == username)
            list.append(a);
    return list;
}

QList<Ad> Database::getAllAds() const
{
    return ads.values();
}

void Database::addToCart(const QString &username, int adId)
{
    carts[username].append(adId);
}

QList<int> Database::getCart(const QString &username) const
{
    return carts.value(username);
}

void Database::removeFromCart(const QString &username, int adId)
{
    if (!carts.contains(username)) return;
    carts[username].removeAll(adId);
}

void Database::clearCart(const QString &username)
{
    carts[username].clear();
}

void Database::addTransaction(const Transaction &t)
{
    transactions.append(t);
}

QList<Transaction> Database::getTransactions(const QString &username) const
{
    QList<Transaction> list;
    for (const auto &t : transactions)
        if (t.username == username)
            list.append(t);
    return list;
}

void Database::addPurchaseRecord(const PurchaseRecord &p)
{
    purchases.append(p);
}

QList<PurchaseRecord> Database::getPurchases(const QString &username) const
{
    QList<PurchaseRecord> list;
    for (const auto &p : purchases)
        if (p.buyer == username)
            list.append(p);
    return list;
}

QList<PurchaseRecord> Database::getSales(const QString &username) const
{
    QList<PurchaseRecord> list;
    for (const auto &p : purchases)
        if (p.seller == username)
            list.append(p);
    return list;
}

AdminStats Database::getAdminStats() const
{
    AdminStats s;
    s.totalUsers = users.size();
    s.totalAds = ads.size();
    s.pendingAds = getPendingAdsCount();
    s.approvedAds = getApprovedAdsCount();
    s.rejectedAds = getRejectedAdsCount();
    s.totalTransactions = transactions.size();
    s.totalPurchases = purchases.size();
    return s;
}

int Database::getPendingAdsCount() const
{
    int c = 0;
    for (const auto &a : ads.values())
        if (a.status == "Pending") c++;
    return c;
}

int Database::getApprovedAdsCount() const
{
    int c = 0;
    for (const auto &a : ads.values())
        if (a.status == "Approved") c++;
    return c;
}

int Database::getRejectedAdsCount() const
{
    int c = 0;
    for (const auto &a : ads.values())
        if (a.status == "Rejected") c++;
    return c;
}

void Database::saveToFile(const QString &path)
{
    QJsonObject root;

    QJsonArray usersArr;
    for (const auto &u : users.values()) {
        QJsonObject o;
        o["username"] = u.username;
        o["passwordHash"] = u.passwordHash;
        o["name"] = u.name;
        o["email"] = u.email;
        o["phone"] = u.phone;
        o["joinDate"] = u.joinDate;
        o["walletBalance"] = u.walletBalance;
        o["adsCount"] = u.adsCount;
        o["purchasesCount"] = u.purchasesCount;
        o["salesCount"] = u.salesCount;
        o["isAdmin"] = u.isAdmin;
        usersArr.append(o);
    }
    root["users"] = usersArr;

    QJsonArray adsArr;
    for (const auto &a : ads.values()) {
        QJsonObject o;
        o["id"] = a.id;
        o["owner"] = a.owner;
        o["title"] = a.title;
        o["description"] = a.description;
        o["price"] = a.price;
        o["category"] = a.category;
        o["status"] = a.status;
        o["imageBase64"] = a.imageBase64;
        o["createdAt"] = a.createdAt;
        o["updatedAt"] = a.updatedAt;
        adsArr.append(o);
    }
    root["ads"] = adsArr;

    QJsonArray cartsArr;
    for (auto it = carts.begin(); it != carts.end(); ++it) {
        QJsonObject o;
        o["username"] = it.key();
        QJsonArray arr;
        for (int id : it.value())
            arr.append(id);
        o["items"] = arr;
        cartsArr.append(o);
    }
    root["carts"] = cartsArr;

    QJsonArray transArr;
    for (const auto &t : transactions) {
        QJsonObject o;
        o["username"] = t.username;
        o["type"] = t.type;
        o["amount"] = t.amount;
        o["timestamp"] = t.timestamp;
        o["description"] = t.description;
        o["relatedAdTitle"] = t.relatedAdTitle;
        o["relatedAdId"] = t.relatedAdId;
        transArr.append(o);
    }
    root["transactions"] = transArr;

    QJsonArray purArr;
    for (const auto &p : purchases) {
        QJsonObject o;
        o["buyer"] = p.buyer;
        o["seller"] = p.seller;
        o["title"] = p.title;
        o["price"] = p.price;
        o["date"] = p.date;
        o["adId"] = p.adId;
        purArr.append(o);
    }
    root["purchases"] = purArr;

    QJsonDocument doc(root);
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
        file.write(doc.toJson());
}

void Database::loadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        return;

    QJsonObject root = doc.object();

    users.clear();
    ads.clear();
    carts.clear();
    transactions.clear();
    purchases.clear();

    QJsonArray usersArr = root["users"].toArray();
    for (const auto &v : usersArr) {
        QJsonObject o = v.toObject();
        User u;
        u.username = o["username"].toString();
        u.passwordHash = o["passwordHash"].toString();
        u.name = o["name"].toString();
        u.email = o["email"].toString();
        u.phone = o["phone"].toString();
        u.joinDate = o["joinDate"].toString();
        u.walletBalance = o["walletBalance"].toDouble();
        u.adsCount = o["adsCount"].toInt();
        u.purchasesCount = o["purchasesCount"].toInt();
        u.salesCount = o["salesCount"].toInt();
        u.isAdmin = o["isAdmin"].toBool();
        users[u.username] = u;
    }

    QJsonArray adsArr = root["ads"].toArray();
    for (const auto &v : adsArr) {
        QJsonObject o = v.toObject();
        Ad a;
        a.id = o["id"].toInt();
        a.owner = o["owner"].toString();
        a.title = o["title"].toString();
        a.description = o["description"].toString();
        a.price = o["price"].toDouble();
        a.category = o["category"].toString();
        a.status = o["status"].toString();
        a.imageBase64 = o["imageBase64"].toString();
        a.createdAt = o["createdAt"].toString();
        a.updatedAt = o["updatedAt"].toString();
        ads[a.id] = a;
        if (a.id >= nextAdId)
            nextAdId = a.id + 1;
    }

    QJsonArray cartsArr = root["carts"].toArray();
    for (const auto &v : cartsArr) {
        QJsonObject o = v.toObject();
        QString user = o["username"].toString();
        QJsonArray arr = o["items"].toArray();
        QList<int> list;
        for (const auto &x : arr)
            list.append(x.toInt());
        carts[user] = list;
    }

    QJsonArray transArr = root["transactions"].toArray();
    for (const auto &v : transArr) {
        QJsonObject o = v.toObject();
        Transaction t;
        t.username = o["username"].toString();
        t.type = o["type"].toString();
        t.amount = o["amount"].toDouble();
        t.timestamp = o["timestamp"].toString();
        t.description = o["description"].toString();
        t.relatedAdTitle = o["relatedAdTitle"].toString();
        t.relatedAdId = o["relatedAdId"].toInt();
        transactions.append(t);
    }

    QJsonArray purArr = root["purchases"].toArray();
    for (const auto &v : purArr) {
        QJsonObject o = v.toObject();
        PurchaseRecord p;
        p.buyer = o["buyer"].toString();
        p.seller = o["seller"].toString();
        p.title = o["title"].toString();
        p.price = o["price"].toDouble();
        p.date = o["date"].toString();
        p.adId = o["adId"].toInt();
        purchases.append(p);
    }
}
