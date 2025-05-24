#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QUrl>
#include <QDesktopServices>
#include <QImage>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>
#include <QUrlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnDownload_clicked() {

    QString urlString = ui->lineURL->text();
    QUrl myURL(urlString);

    if (myURL.isValid() && (myURL.scheme() == "http" || myURL.scheme() == "https")) {

        QString path = myURL.path().toLower();

       if (path.endsWith(".jpg") || path.endsWith(".jpeg") || path.endsWith(".png") ||
            path.endsWith(".bmp") || path.endsWith(".webp")) {
            //path
            QString fileName = QFileInfo(path).fileName();
            QString savePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/" + fileName;
            QNetworkAccessManager *manager = new QNetworkAccessManager(this);
            QNetworkRequest request(myURL);

            QNetworkReply *reply = manager->get(request);

            QObject::connect(reply, &QNetworkReply::finished, [=]() {
                if (reply->error() == QNetworkReply::NoError) {
                    QByteArray imageData = reply->readAll();
                    QImage image;
                    if (image.loadFromData(imageData)) {
                        if (image.save(savePath)) {
                            QMessageBox::information(this, "Success",
                                                     QString("Image saved to:\n%1").arg(savePath));
                            //go negative
                            QImage negativeImage=image;
                            negativeImage.invertPixels(QImage::InvertRgb);

                            QFileInfo fileInfo(savePath);
                            QString negativePath=fileInfo.path() + "/" + fileInfo.completeBaseName()
                                                                  + "_negative." + fileInfo.suffix();
                            if(negativeImage.save(negativePath)){ //show

                                QMessageBox::information(this, "Success",
                                    QString("Negative version saved to:\n%1").arg(negativePath));

                                //open pictures
                                QDesktopServices::openUrl(QUrl::fromLocalFile(savePath));
                                QDesktopServices::openUrl(QUrl::fromLocalFile(negativePath));

                            }

                        } else {
                            QMessageBox::warning(this, "Error", "Failed to save image");
                        }
                    } else {
                        QMessageBox::warning(this, "Error", "Downloaded data is not a valid image");
                    }
                } else {
                    QMessageBox::critical(this, "Download Failed",
                                          QString("Error: %1").arg(reply->errorString()));
                }

                reply->deleteLater();
                manager->deleteLater();
            });

       } else {QMessageBox::warning(this, "Invalid URL", "Please enter a valid HTTP or HTTPS URL.");}
    }
}



