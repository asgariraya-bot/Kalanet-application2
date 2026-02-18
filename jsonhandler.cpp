#include "jsonhandler.h"
#include "database.h"
#include <QJsonArray>
#include <QCryptographicHash>
#include <QDateTime>

JsonHandler::JsonHandler()
{
}

QString JsonHandler::hashPassword(const QString &plain) const
{
    QByteArray b = plain.toUtf8();
    QByteArray h = QCryptographicHash::hash(b, QCryptographicHash::Sha256);
    return QString::fromLatin1(h.toHex());
}

QString JsonHandler::now() const
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

QJsonObject JsonHandler::handleRequest(const QJsonObject &req)
{
    QString type = req.value("type").toString();
    if (type == "login") return handleLogin(req);
    if (type == "signup") return handleSignup(req);

    if (type == "add_ad") return handleAddAd(req);
    if (type == "get_ads") return handleGetAds(req);

    if (type == "add_to_cart") return handleAddToCart(req);
    if (type == "get_cart") return handleGetCart(req);
    if (type == "remove_from_cart") return handleRemoveFromCart(req);
    if (type == "purchase_cart") return handlePurchaseCart(req);

    if (type == "get_wallet") return handleGetWallet(req);
    if (type == "wallet_deposit") return handleWalletDeposit(req);
    if (type == "wallet_withdraw") return handleWalletWithdraw(req);
    if (type == "get_transactions") return handleGetTransactions(req);

    if (type == "get_profile") return handleGetProfile(req);
    if (type == "get_user_ads") return handleGetUserAds(req);
    if (type == "get_user_purchases") return handleGetUserPurchases(req);
    if (type == "get_user_sales") return handleGetUserSales(req);

    if (type == "get_pending_ads") return handleGetPendingAds(req);
    if (type == "get_approved_ads") return handleGetApprovedAds(req);
    if (type == "get_rejected_ads") return handleGetRejectedAds(req);
    if (type == "approve_ad") return handleApproveAd(req);
    if (type == "reject_ad") return handleRejectAd(req);
    if (type == "get_admin_stats") return handleGetAdminStats(req);

    QJsonObject res;
    res["type"] = "error";
    res["message"] = "Unknown request type";
    return res;
}

QJsonObject JsonHandler::handleLogin(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "login_response";

    QString username = req.value("username").toString();
    QString password = req.value("password").toString();
    QString hash = hashPassword(password);

    Database &db = Database::instance();
    if (!db.userExists(username) || !db.checkPassword(username, hash)) {
        res["success"] = false;
        res["message"] = "Invalid username or password";
        return res;
    }

    User u = db.getUser(username);
    res["success"] = true;
    res["message"] = "Login successful";
    res["is_admin"] = u.isAdmin;
    return res;
}

QJsonObject JsonHandler::handleSignup(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "signup_response";

    QString username = req.value("username").toString();
    QString password = req.value("password").toString();
    QString name     = req.value("name").toString();
    QString email    = req.value("email").toString();
    QString phone    = req.value("phone").toString();

    Database &db = Database::instance();
    if (db.userExists(username)) {
        res["success"] = false;
        res["message"] = "Username already exists";
        return res;
    }

    User u;
    u.username = username;
    u.passwordHash = hashPassword(password);
    u.name = name;
    u.email = email;
    u.phone = phone;
    u.joinDate = now();
    u.walletBalance = 0.0;
    u.adsCount = 0;
    u.purchasesCount = 0;
    u.salesCount = 0;
    u.isAdmin = false;

    db.addUser(u);

    res["success"] = true;
    res["message"] = "Signup successful";
    return res;
}

QJsonObject JsonHandler::handleAddAd(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "add_ad_response";

    QString username   = req.value("username").toString();
    QString title      = req.value("title").toString();
    QString description= req.value("description").toString();
    double price       = req.value("price").toDouble();
    QString category   = req.value("category").toString();
    QString imageBase64= req.value("image_base64").toString();

    Database &db = Database::instance();
    if (!db.userExists(username)) {
        res["success"] = false;
        res["message"] = "User not found";
        return res;
    }

    Ad ad;
    ad.id = 0;
    ad.owner = username;
    ad.title = title;
    ad.description = description;
    ad.price = price;
    ad.category = category;
    ad.status = "Pending";
    ad.imageBase64 = imageBase64;
    ad.createdAt = now();
    ad.updatedAt = ad.createdAt;

    int id = db.addAd(ad);

    User u = db.getUser(username);
    u.adsCount += 1;
    db.updateUser(u);

    res["success"] = true;
    res["message"] = "Ad submitted";
    res["ad_id"] = id;
    return res;
}

QJsonObject JsonHandler::handleGetAds(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "get_ads_response";

    QString status = req.value("status").toString("Approved");

    Database &db = Database::instance();
    QList<Ad> list = db.getAdsByStatus(status);

    QJsonArray arr;
    for (const auto &a : list) {
        QJsonObject o;
        o["id"] = a.id;
        o["owner"] = a.owner;
        o["title"] = a.title;
        o["description"] = a.description;
        o["price"] = a.price;
        o["category"] = a.category;
        o["status"] = a.status;
        o["image_base64"] = a.imageBase64;
        o["created_at"] = a.createdAt;
        o["updated_at"] = a.updatedAt;
        arr.append(o);
    }

    res["ads"] = arr;
    return res;
}

QJsonObject JsonHandler::handleAddToCart(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "add_to_cart_response";

    QString username = req.value("username").toString();
    int adId = req.value("ad_id").toInt();

    Database &db = Database::instance();
    if (!db.userExists(username)) {
        res["success"] = false;
        res["message"] = "User not found";
        return res;
    }

    Ad ad = db.getAd(adId);
    if (ad.id == 0 || ad.status != "Approved") {
        res["success"] = false;
        res["message"] = "Ad not available";
        return res;
    }

    db.addToCart(username, adId);

    res["success"] = true;
    res["message"] = "Added to cart";
    return res;
}

QJsonObject JsonHandler::handleGetCart(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "get_cart_response";

    QString username = req.value("username").toString();
    Database &db = Database::instance();

    QList<int> ids = db.getCart(username);
    QJsonArray arr;
    double total = 0.0;

    for (int id : ids) {
        Ad a = db.getAd(id);
        if (a.id == 0 || a.status != "Approved")
            continue;
        QJsonObject o;
        o["id"] = a.id;
        o["title"] = a.title;
        o["price"] = a.price;
        o["category"] = a.category;
        o["owner"] = a.owner;
        arr.append(o);
        total += a.price;
    }

    res["items"] = arr;
    res["total_price"] = total;
    return res;
}

QJsonObject JsonHandler::handleRemoveFromCart(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "remove_from_cart_response";

    QString username = req.value("username").toString();
    int adId = req.value("ad_id").toInt();

    Database &db = Database::instance();
    db.removeFromCart(username, adId);

    res["success"] = true;
    res["message"] = "Removed from cart";
    return res;
}

QJsonObject JsonHandler::handlePurchaseCart(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "purchase_cart_response";

    QString username = req.value("username").toString();
    Database &db = Database::instance();

    if (!db.userExists(username)) {
        res["success"] = false;
        res["message"] = "User not found";
        return res;
    }

    User buyer = db.getUser(username);
    QList<int> ids = db.getCart(username);

    double total = 0.0;
    QList<Ad> adsToBuy;
    for (int id : ids) {
        Ad a = db.getAd(id);
        if (a.id == 0 || a.status != "Approved")
            continue;
        adsToBuy.append(a);
        total += a.price;
    }

    if (adsToBuy.isEmpty()) {
        res["success"] = false;
        res["message"] = "Cart is empty";
        return res;
    }

    if (buyer.walletBalance < total) {
        res["success"] = false;
        res["message"] = "Insufficient balance";
        return res;
    }

    buyer.walletBalance -= total;
    buyer.purchasesCount += adsToBuy.size();
    db.updateUser(buyer);

    for (const auto &a : adsToBuy) {
        User seller = db.getUser(a.owner);
        seller.walletBalance += a.price;
        seller.salesCount += 1;
        db.updateUser(seller);

        PurchaseRecord p;
        p.buyer = buyer.username;
        p.seller = seller.username;
        p.title = a.title;
        p.price = a.price;
        p.date = now();
        p.adId = a.id;
        db.addPurchaseRecord(p);

        Transaction tb;
        tb.username = buyer.username;
        tb.type = "purchase";
        tb.amount = -a.price;
        tb.timestamp = p.date;
        tb.description = QString("Purchase ad %1").arg(a.id);
        tb.relatedAdTitle = a.title;
        tb.relatedAdId = a.id;
        db.addTransaction(tb);

        Transaction ts;
        ts.username = seller.username;
        ts.type = "sale";
        ts.amount = a.price;
        ts.timestamp = p.date;
        ts.description = QString("Sold ad %1").arg(a.id);
        ts.relatedAdTitle = a.title;
        ts.relatedAdId = a.id;
        db.addTransaction(ts);
    }

    db.clearCart(username);

    res["success"] = true;
    res["message"] = "Purchase successful";
    res["new_balance"] = buyer.walletBalance;
    return res;
}

QJsonObject JsonHandler::handleGetWallet(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "get_wallet_response";

    QString username = req.value("username").toString();
    Database &db = Database::instance();

    if (!db.userExists(username)) {
        res["balance"] = 0.0;
        return res;
    }

    User u = db.getUser(username);
    res["balance"] = u.walletBalance;
    return res;
}

QJsonObject JsonHandler::handleWalletDeposit(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "wallet_deposit_response";

    QString username = req.value("username").toString();
    double amount = req.value("amount").toDouble();

    Database &db = Database::instance();
    if (!db.userExists(username) || amount <= 0) {
        res["success"] = false;
        res["message"] = "Invalid request";
        return res;
    }

    User u = db.getUser(username);
    u.walletBalance += amount;
    db.updateUser(u);

    Transaction t;
    t.username = username;
    t.type = "deposit";
    t.amount = amount;
    t.timestamp = now();
    t.description = "Wallet deposit";
    t.relatedAdTitle = "";
    t.relatedAdId = 0;
    db.addTransaction(t);

    res["success"] = true;
    res["message"] = "Deposit successful";
    res["new_balance"] = u.walletBalance;
    return res;
}

QJsonObject JsonHandler::handleWalletWithdraw(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "wallet_withdraw_response";

    QString username = req.value("username").toString();
    double amount = req.value("amount").toDouble();

    Database &db = Database::instance();
    if (!db.userExists(username) || amount <= 0) {
        res["success"] = false;
        res["message"] = "Invalid request";
        return res;
    }

    User u = db.getUser(username);
    if (u.walletBalance < amount) {
        res["success"] = false;
        res["message"] = "Insufficient balance";
        return res;
    }

    u.walletBalance -= amount;
    db.updateUser(u);

    Transaction t;
    t.username = username;
    t.type = "withdraw";
    t.amount = -amount;
    t.timestamp = now();
    t.description = "Wallet withdraw";
    t.relatedAdTitle = "";
    t.relatedAdId = 0;
    db.addTransaction(t);

    res["success"] = true;
    res["message"] = "Withdraw successful";
    res["new_balance"] = u.walletBalance;
    return res;
}

QJsonObject JsonHandler::handleGetTransactions(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "get_transactions_response";

    QString username = req.value("username").toString();
    Database &db = Database::instance();

    QList<Transaction> list = db.getTransactions(username);
    QJsonArray arr;
    for (const auto &t : list) {
        QJsonObject o;
        o["type"] = t.type;
        o["amount"] = t.amount;
        o["timestamp"] = t.timestamp;
        o["description"] = t.description;
        o["related_ad_title"] = t.relatedAdTitle;
        o["related_ad_id"] = t.relatedAdId;
        arr.append(o);
    }

    res["transactions"] = arr;
    return res;
}

QJsonObject JsonHandler::handleGetProfile(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "get_profile_response";

    QString username = req.value("username").toString();
    Database &db = Database::instance();

    if (!db.userExists(username)) {
        res["name"] = "";
        res["email"] = "";
        res["phone"] = "";
        res["join_date"] = "";
        res["ads_count"] = 0;
        res["purchases"] = 0;
        res["sales"] = 0;
        return res;
    }

    User u = db.getUser(username);
    res["name"] = u.name;
    res["email"] = u.email;
    res["phone"] = u.phone;
    res["join_date"] = u.joinDate;
    res["ads_count"] = u.adsCount;
    res["purchases"] = u.purchasesCount;
    res["sales"] = u.salesCount;
    return res;
}

QJsonObject JsonHandler::handleGetUserAds(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "get_user_ads_response";

    QString username = req.value("username").toString();
    Database &db = Database::instance();

    QList<Ad> list = db.getUserAds(username);
    QJsonArray arr;
    for (const auto &a : list) {
        QJsonObject o;
        o["id"] = a.id;
        o["title"] = a.title;
        o["price"] = a.price;
        o["category"] = a.category;
        o["status"] = a.status;
        arr.append(o);
    }

    res["ads"] = arr;
    return res;
}

QJsonObject JsonHandler::handleGetUserPurchases(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "get_user_purchases_response";

    QString username = req.value("username").toString();
    Database &db = Database::instance();

    QList<PurchaseRecord> list = db.getPurchases(username);
    QJsonArray arr;
    for (const auto &p : list) {
        QJsonObject o;
        o["title"] = p.title;
        o["price"] = p.price;
        o["date"] = p.date;
        o["seller"] = p.seller;
        arr.append(o);
    }

    res["purchases"] = arr;
    return res;
}

QJsonObject JsonHandler::handleGetUserSales(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "get_user_sales_response";

    QString username = req.value("username").toString();
    Database &db = Database::instance();

    QList<PurchaseRecord> list = db.getSales(username);
    QJsonArray arr;
    for (const auto &p : list) {
        QJsonObject o;
        o["title"] = p.title;
        o["price"] = p.price;
        o["date"] = p.date;
        o["buyer"] = p.buyer;
        arr.append(o);
    }

    res["sales"] = arr;
    return res;
}

QJsonObject JsonHandler::handleGetPendingAds(const QJsonObject &)
{
    QJsonObject res;
    res["type"] = "get_pending_ads_response";

    Database &db = Database::instance();
    QList<Ad> list = db.getAdsByStatus("Pending");

    QJsonArray arr;
    for (const auto &a : list) {
        QJsonObject o;
        o["id"] = a.id;
        o["title"] = a.title;
        o["price"] = a.price;
        o["category"] = a.category;
        o["owner"] = a.owner;
        arr.append(o);
    }

    res["ads"] = arr;
    return res;
}

QJsonObject JsonHandler::handleGetApprovedAds(const QJsonObject &)
{
    QJsonObject res;
    res["type"] = "get_approved_ads_response";

    Database &db = Database::instance();
    QList<Ad> list = db.getAdsByStatus("Approved");

    QJsonArray arr;
    for (const auto &a : list) {
        QJsonObject o;
        o["id"] = a.id;
        o["title"] = a.title;
        o["price"] = a.price;
        o["category"] = a.category;
        o["owner"] = a.owner;
        arr.append(o);
    }

    res["ads"] = arr;
    return res;
}

QJsonObject JsonHandler::handleGetRejectedAds(const QJsonObject &)
{
    QJsonObject res;
    res["type"] = "get_rejected_ads_response";

    Database &db = Database::instance();
    QList<Ad> list = db.getAdsByStatus("Rejected");

    QJsonArray arr;
    for (const auto &a : list) {
        QJsonObject o;
        o["id"] = a.id;
        o["title"] = a.title;
        o["price"] = a.price;
        o["category"] = a.category;
        o["owner"] = a.owner;
        arr.append(o);
    }

    res["ads"] = arr;
    return res;
}

QJsonObject JsonHandler::handleApproveAd(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "approve_ad_response";

    int adId = req.value("ad_id").toInt();
    Database &db = Database::instance();

    Ad a = db.getAd(adId);
    if (a.id == 0) {
        res["success"] = false;
        res["message"] = "Ad not found";
        return res;
    }

    db.updateAdStatus(adId, "Approved");

    res["success"] = true;
    res["message"] = "Ad approved";
    return res;
}

QJsonObject JsonHandler::handleRejectAd(const QJsonObject &req)
{
    QJsonObject res;
    res["type"] = "reject_ad_response";

    int adId = req.value("ad_id").toInt();
    Database &db = Database::instance();

    Ad a = db.getAd(adId);
    if (a.id == 0) {
        res["success"] = false;
        res["message"] = "Ad not found";
        return res;
    }

    db.updateAdStatus(adId, "Rejected");

    res["success"] = true;
    res["message"] = "Ad rejected";
    return res;
}

QJsonObject JsonHandler::handleGetAdminStats(const QJsonObject &)
{
    QJsonObject res;
    res["type"] = "get_admin_stats_response";

    Database &db = Database::instance();
    AdminStats s = db.getAdminStats();

    res["total_users"] = s.totalUsers;
    res["total_ads"] = s.totalAds;
    res["pending_ads"] = s.pendingAds;
    res["approved_ads"] = s.approvedAds;
    res["rejected_ads"] = s.rejectedAds;
    res["total_transactions"] = s.totalTransactions;
    res["total_purchases"] = s.totalPurchases;

    return res;
}
