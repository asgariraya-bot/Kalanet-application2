#ifndef AD_H
#define AD_H

#include <QString>
#include <QImage>
#include <QDateTime>
#include <vector>


enum class AdStatus {
    Pending,
    Approved,
    Sold
};

class Ad {
private:
    int adId;
    QString title;
    QString description;
    double price;
    QString category;
    QImage image;
    QString sellerUsername;
    AdStatus status;
    QDateTime timestamp;

public:
    Ad(int id, QString t, QString desc, double p, QString cat, QImage img, QString seller);

    // Getters
    int getId() const;
    QString getTitle() const;
    double getPrice() const;
    AdStatus getStatus() const;
    QString getSeller() const;
    QString getCategory() const;
    QImage getImage() const;
    QString getFormattedTime() const;

    // Setters
    void setStatus(AdStatus newStatus);
};


class AdManager {
private:
    std::vector<Ad> allAds;

public:

    bool registerAd(const Ad& newAd);
    void saveToFile(const QString& fileName);
    void loadFromFile(const QString& fileName);
    std::vector<Ad> getApprovedAds() const;

    void updateAdStatus(int id, AdStatus newStatus);
};

#endif
