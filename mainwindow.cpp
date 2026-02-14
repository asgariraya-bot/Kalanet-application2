#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include"ad.h"
#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myManager.loadFromFile("ads_data.bin");
    updateAdsDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_selectImgButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Ad Image", "", "Images (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        QImage tempImg(fileName);
        ui->imgPreviewLabel->setPixmap(QPixmap::fromImage(tempImg));
        this->selectedImage = tempImg;
    }
}


void MainWindow::on_submitAdButton_clicked()
{
    QString title = ui->titleLineEdit->text();
    QString desc = ui->descTextEdit->toPlainText();
    double price = ui->priceSpinBox->value();
    QString cat = ui->categoryComboBox->currentText();


    Ad newAd(1, title, desc, price, cat, this->selectedImage, "CurrentUser");


    if(myManager.registerAd(newAd)) {
        ui->statusbar->showMessage("Ad Registered Successfully!", 3000);
    }
    myManager.saveToFile("ads_data.bin");
    updateAdsDisplay();
}


void MainWindow::updateAdsDisplay() {
    ui->adsListWidget->clear();


    auto approvedAds = myManager.getApprovedAds();

    for (const auto& ad : approvedAds) {
        QString itemText = ad.getTitle() + " - " + QString::number(ad.getPrice()) + " $";
        ui->adsListWidget->addItem(itemText);
    }
}

void MainWindow::on_adsListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // Find the selected row index
    int index = ui->adsListWidget->currentRow();

    // Get the list of approved ads from the manager
    std::vector<Ad> allAds = myManager.getApprovedAds();

    if (index >= 0 && index < static_cast<int>(allAds.size())) {
        Ad selectedAd = allAds[index];

        // Prepare the detail text in English
        QString detailText = "Title: " + selectedAd.getTitle() + "\n" +
                             "Price: " + QString::number(selectedAd.getPrice()) + " $\n" +
                             "Category: " + selectedAd.getCategory() + "\n" +
                             "Seller: " + selectedAd.getSeller() + "\n" +
                             "Time: " + selectedAd.getFormattedTime();

        QMessageBox msgBox;
        msgBox.setWindowTitle("Ad Details");
        msgBox.setText(detailText);

        // Show the ad's image in the message box
        if (!selectedAd.getImage().isNull()) {
            msgBox.setIconPixmap(QPixmap::fromImage(selectedAd.getImage()).scaled(200, 200, Qt::KeepAspectRatio));
        }

        msgBox.exec();
    }
}
