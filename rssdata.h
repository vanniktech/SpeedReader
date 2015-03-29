#ifndef RSSDATABASE_H
#define RSSDATABASE_H

#include <QObject>
#include <QUuid>

#include "rssselectiondata.h"
#include "rssitem.h"
#include "databasehelper.h"
#include "lib/VNTRSSReader/vntrsschannel.h"
#include "rsschannel.h"

class RSSData : public QObject
{
    Q_OBJECT
public:
    explicit RSSData(QObject *parent = 0);
    void deleteData();
    void deleteRSSSites(QList<QString> rssSite);
    bool setSpeedRead(QUuid id, bool speedread);

    int unspeedreadAllCount();
    int unspeedreadTodayCount();
    int unspeedreadYesterdayCount();
    QList<RSSChannel> getChannels();

    QList<RSSItem*> getRSSItems(RSSSelectionData rssSelectionData);

signals:
    void finished();
    void failedToOpenDatabase();
    void failedToLoadRSSChannel(QString errorMessage);

public slots:
    void loadedRSS(QList<VNTRSSChannel*> rssChannels);

private:
    DatabaseHelper* mDatabaseHelper;

    void makeImageFolder();
    void removeAllImages();
    QString uuidToString(QUuid uuid);
    QDateTime getTodayDateTime();
    QDateTime getYesterdayDateTime();
};

#endif // RSSDATABASE_H
