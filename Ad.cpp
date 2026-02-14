#include "ad.h"
#include <QDateTime>
#include <QFile>
#include <QDataStream>

Ad::Ad(int id, QString t, QString desc, double p, QString cat, QImage img, QString seller)
    : adId(id), title(t), description(desc), price(p), category(cat),
    sellerUsername(seller), status(AdStatus::Pending) {


        if(!img.isNull()) {
        image = img.scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }


        timestamp = QDateTime::currentDateTime();
}


int Ad::getId() const { return adId; }
QString Ad::getTitle() const { return title; }
double Ad::getPrice() const { return price; }
AdStatus Ad::getStatus() const { return status; }
QString Ad::getSeller() const { return sellerUsername; }
QString Ad::getCategory() const { return category; }
QImage Ad::getImage() const { return image; }

QString Ad::getFormattedTime() const {
    return timestamp.toString("yyyy-MM-dd HH:mm");
}

void Ad::setStatus(AdStatus newStatus) {
    status = newStatus;
}


bool AdManager::registerAd(const Ad& newAd) {

        for (const auto& ad : allAds) {
        if (ad.getSeller() == newAd.getSeller() && ad.getTitle() == newAd.getTitle()) {
            return false;
        }
    }
    allAds.push_back(newAd);
    return true;
}

std::vector<Ad> AdManager::getApprovedAds() const {
    std::vector<Ad> approved;
    for (const auto& ad : allAds) {

            if (ad.getStatus() == AdStatus::Approved) {
            approved.push_back(ad);
        }
    }
    return approved;
}

void AdManager::updateAdStatus(int id, AdStatus newStatus) {
    for (auto& ad : allAds) {
        if (ad.getId() == id) {
            ad.setStatus(newStatus);
            break;
        }
    }
}


void AdManager::saveToFile(const QString& fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << (int)allAds.size();
        for (const auto& ad : allAds) {

            out << ad.getTitle() << ad.getPrice() << ad.getCategory() << ad.getImage();
        }
        file.close();
    }
}

void AdManager::loadFromFile(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    QDataStream in(&file);
    int size;
    in >> size;
    allAds.clear();

    for (int i = 0; i < size; ++i) {
        QString title, cat;
        double price;
        QImage img;
        in >> title >> price >> cat >> img;


        Ad ad(i, title, "No description", price, cat, img, "Admin");
        ad.setStatus(AdStatus::Approved);
        allAds.push_back(ad);
    }
    file.close();
}
