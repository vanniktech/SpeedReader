#ifndef CHECKFORUPDATES_H
#define CHECKFORUPDATES_H

#include <QObject>
#include <QNetworkAccessManager>

class CheckForUpdates : public QObject {
    Q_OBJECT

public:
    explicit CheckForUpdates(QString urlTxt, QString urlSite, QObject *parent = 0);
    void checkForUpdates(bool silent = false);

private slots:
    void updateReplyFinished(QNetworkReply* networkReply);

private:
    QNetworkAccessManager* mNetworkAccessManagerUpdate;
    bool mSilent;
    QString mUrlTxt, mUrlSite;
};

#endif // CHECKFORUPDATES_H
