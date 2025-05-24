#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QString>
#include <QImage>

class ImageDownloader : public  QObject
{
    Q_OBJECT
public:
    explicit ImageDownloader(QObject *parent = nullptr) :
        QObject(parent) {manager =new QNetworkAccessManager(this);}
    void download(const QUrl &url, const QString &savePath){
        QNetworkReply *reply = manager->get(QNetworkRequest(url));

        QObject::connect(reply, &QNetworkReply::finished, [=]() {
            bool success = false;
            if (!reply->error()) {
                QImage image;
                if (image.loadFromData(reply->readAll())) {
                    success = image.save(savePath);
                }
            }
            emit finished(success, savePath);
            reply->deleteLater();
        });
    }
signals:
    void finished(bool success, const QString &path);
private:
    QNetworkAccessManager *manager;
};

#endif // IMAGEDOWNLOADER_H
