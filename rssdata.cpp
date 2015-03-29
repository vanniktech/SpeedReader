#include "rssdata.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDir>

static const QString IMAGE_FOLDER_PATH = QDir::current().absolutePath().append("/rss_images/");

RSSData::RSSData(QObject *parent) : QObject(parent) {
    mDatabaseHelper = new DatabaseHelper(this);
    QObject::connect(mDatabaseHelper, SIGNAL(failedToOpen()), this, SIGNAL(failedToOpenDatabase()));
}

void RSSData::loadedRSS(QList<VNTRSSChannel*> rssChannels) {
    foreach (VNTRSSChannel* rssChannel, rssChannels) {
        if (rssChannel->hasError()) {
            emit failedToLoadRSSChannel(rssChannel->getErrorMessage());
            continue;
        } else if (rssChannel->getTitle().isEmpty()) continue;

        QUuid rssChannelId;
        QSqlQuery queryChannelExists;
        queryChannelExists.prepare("SELECT rss_channel_id, title, description, date FROM rss_channel WHERE rss_site = ?");
        queryChannelExists.addBindValue(rssChannel->getRSSSite());
        queryChannelExists.exec();

        if (queryChannelExists.next()) {    // does exist
            rssChannelId = queryChannelExists.value("rss_channel_id").value<QUuid>();

            QMap<QString, QVariant> contentValues;

            if (queryChannelExists.value("title").value<QString>() != rssChannel->getTitle()) contentValues.insert("title", rssChannel->getTitle());
            if (queryChannelExists.value("description").value<QString>() != rssChannel->getPlainDescription()) contentValues.insert("description", rssChannel->getPlainDescription());
            if (queryChannelExists.value("date").value<uint>() != rssChannel->getDate().toTime_t()) contentValues.insert("date", rssChannel->getDate().toTime_t());

            mDatabaseHelper->update("rss_channel", contentValues, "rss_channel_id", rssChannelId);
        } else {                            // does not exist
            rssChannelId = QUuid::createUuid();
            mDatabaseHelper->insert("rss_channel", QList<QVariant>() << rssChannelId << rssChannel->getTitle() << rssChannel->getPlainDescription() << rssChannel->getLink() << rssChannel->getRSSSite() << "" << rssChannel->getImageUrl() << rssChannel->getDate().toTime_t());
        }

        this->makeImageFolder();

        QString imageFolderPath = QString("%1%2/").arg(IMAGE_FOLDER_PATH, this->uuidToString(rssChannelId));
        if (!QDir(imageFolderPath).exists()) QDir().mkdir(imageFolderPath);

        QList<VNTRSSItem*> rssItems = rssChannel->getRSSItems();
        foreach (VNTRSSItem* rssItem, rssItems) {
            QSqlQuery queryItemExists;
            queryItemExists.prepare("SELECT rss_item_id, title, description, date FROM rss_item WHERE link = ? AND rss_channel_id = ?");
            queryItemExists.addBindValue(rssItem->getLink());
            queryItemExists.addBindValue(rssChannelId);
            queryItemExists.exec();

            if (queryItemExists.next()) {   // already in database
                QUuid rssItemId = queryItemExists.value("rss_item_id").value<QUuid>();

                QMap<QString, QVariant> contentValues;

                if (queryItemExists.value("title").value<QString>() != rssItem->getTitle()) contentValues.insert("title", rssItem->getTitle());
                if (queryItemExists.value("description").value<QString>() != rssItem->getPlainDescription()) contentValues.insert("description", rssItem->getPlainDescription());
                if (queryItemExists.value("date").value<uint>() != rssItem->getDate().toTime_t()) contentValues.insert("date", rssItem->getDate().toTime_t());

                mDatabaseHelper->update("rss_item", contentValues, "rss_item_id", rssItemId);
            } else {                    // not in database
                QList<QVariant> values = QList<QVariant>() << QUuid::createUuid() << rssChannelId << rssItem->getTitle() << rssItem->getPlainDescription() << rssItem->getLink() << rssItem->getDate().toTime_t();
                QString imageFileType = rssItem->getImageFileType();

                if (!imageFileType.isNull() && !imageFileType.isEmpty()) {
                    QString imageFileName = imageFolderPath + QString::number(QDateTime::currentMSecsSinceEpoch()) + QString::number(rand()) + "." + imageFileType;
                    rssItem->getImage().save(imageFileName);
                    values.append(imageFileName);
                } else values.append("");

                values.append(rssItem->getImageUrl());
                values.append(false);
                mDatabaseHelper->insert("rss_item", values);
            }
        }
    }

    qDeleteAll(rssChannels.begin(), rssChannels.end());
    emit finished();
}

QList<RSSChannel> RSSData::getChannels() {
    QList<RSSChannel> rssChannels;
    QSqlQuery queryRSSSites;
    queryRSSSites.exec("SELECT c.title, c.rss_site, c.link, (SELECT COUNT(*) FROM rss_item as i WHERE i.rss_channel_id = c.rss_channel_id AND i.speedread = 0) as count FROM rss_channel as c");

    while (queryRSSSites.next()) {
        RSSChannel rssChannel;
        rssChannel.title = queryRSSSites.value("title").value<QString>();
        rssChannel.unspeedread = queryRSSSites.value("count").value<int>();
        rssChannel.link = queryRSSSites.value("link").value<QString>();
        rssChannel.rssSite = queryRSSSites.value("rss_site").value<QString>();

        rssChannels.append(rssChannel);
    }

    return rssChannels;
}

void RSSData::deleteData() {
    mDatabaseHelper->dropTables();
    mDatabaseHelper->createTables();

    this->removeAllImages();
    this->makeImageFolder();
}

void RSSData::removeAllImages() {
    QDir(IMAGE_FOLDER_PATH).removeRecursively();
}

void RSSData::makeImageFolder() {
    if (!QDir(IMAGE_FOLDER_PATH).exists()) QDir().mkdir(IMAGE_FOLDER_PATH);
}

QString RSSData::uuidToString(QUuid uuid) {
    return uuid.toString().mid(1, 36);
}

void RSSData::deleteRSSSites(QList<QString> rssSites) {
    foreach (QString rssSite, rssSites) {
        QSqlQuery queryRSSChannelId;
        queryRSSChannelId.prepare("SELECT rss_channel_id FROM rss_channel WHERE rss_site = ?");
        queryRSSChannelId.addBindValue(rssSite);
        queryRSSChannelId.exec();

        if (queryRSSChannelId.next()) {
            QUuid rssChannelId = queryRSSChannelId.value("rss_channel_id").value<QUuid>();
            QString imagePath = QString("%1%2").arg(IMAGE_FOLDER_PATH, this->uuidToString(rssChannelId));

            if (QDir(imagePath).removeRecursively()) {
                mDatabaseHelper->beginTransaction();
                QSqlQuery queryDeleteRSSItems;
                queryDeleteRSSItems.prepare("DELETE FROM rss_item WHERE rss_channel_id = ?");
                queryDeleteRSSItems.addBindValue(rssChannelId);
                queryDeleteRSSItems.exec();

                QSqlQuery queryDeleteRSSChannel;
                queryDeleteRSSChannel.prepare("DELETE FROM rss_channel WHERE rss_channel_id = ?");
                queryDeleteRSSChannel.addBindValue(rssChannelId);
                queryDeleteRSSChannel.exec();
                mDatabaseHelper->commitTransaction();
            }
        }
    }
}

bool RSSData::setSpeedRead(QUuid id, bool speedread) {
    return mDatabaseHelper->update("rss_item", "speedread", speedread, "rss_item_id", id);
}

int RSSData::unspeedreadAllCount() {
    return mDatabaseHelper->getSingleInt("SELECT COUNT(*) FROM rss_item WHERE speedread = 0", QList<QVariant>(), 0);
}

int RSSData::unspeedreadTodayCount() {
    return mDatabaseHelper->getSingleInt("SELECT COUNT(*) FROM rss_item WHERE speedread = 0 AND date BETWEEN ? AND ?", QList<QVariant>() << this->getTodayDateTime().toTime_t() << QDateTime::currentMSecsSinceEpoch() / 1000L, 0);
}

int RSSData::unspeedreadYesterdayCount() {
    return mDatabaseHelper->getSingleInt("SELECT COUNT(*) FROM rss_item WHERE speedread = 0 AND date BETWEEN ? AND ?", QList<QVariant>() << this->getYesterdayDateTime().toTime_t() << this->getTodayDateTime().toTime_t(), 0);
}

QDateTime RSSData::getTodayDateTime() {
    QDateTime todayDateTime = QDateTime::currentDateTime();
    todayDateTime.setTime(QTime(0, 0));

    return todayDateTime;
}

QDateTime RSSData::getYesterdayDateTime() {
    QDateTime yesterdayDateTime = QDateTime::currentDateTime().addDays(-1);
    yesterdayDateTime.setTime(QTime(0, 0));

    return yesterdayDateTime;
}

QList<RSSItem*> RSSData::getRSSItems(RSSSelectionData rssSelectionData) {
    QList<RSSItem*> rssItems;
    QSqlQuery query;

    if (rssSelectionData.selection == ALL) query.prepare("SELECT rss_item_id, title, description, link, image_internet, image_local, date, speedread FROM rss_item ORDER BY date DESC");
    else if (rssSelectionData.selection == UNSPEEDREAD) query.prepare("SELECT rss_item_id, title, description, link, image_internet, image_local, date, speedread FROM rss_item WHERE speedread = 0 ORDER BY date DESC");
    else if (rssSelectionData.selection == TODAY) {
        query.prepare("SELECT rss_item_id, title, description, link, image_internet, image_local, date, speedread FROM rss_item WHERE date BETWEEN ? AND ? ORDER BY date DESC");
        query.addBindValue(this->getTodayDateTime().toTime_t());
        query.addBindValue(QDateTime::currentMSecsSinceEpoch() / 1000L);
    } else if (rssSelectionData.selection == YESTERDAY) {
        query.prepare("SELECT rss_item_id, title, description, link, image_internet, image_local, date, speedread FROM rss_item WHERE date BETWEEN ? AND ? ORDER BY date DESC");
        query.addBindValue(this->getYesterdayDateTime().toTime_t());
        query.addBindValue(this->getTodayDateTime().toTime_t());
    } else if (rssSelectionData.selection == SITE) {
        query.prepare("SELECT i.rss_item_id, i.title, i.description, i.link, i.image_local, i.image_internet, i.date, i.speedread FROM rss_item i, rss_channel c WHERE i.rss_channel_id = c.rss_channel_id AND c.rss_site = ? ORDER BY i.date DESC");
        query.addBindValue(rssSelectionData.rssSite);
    }

    query.exec();

    while (query.next())
        rssItems.append(new RSSItem(query));

    return rssItems;
}
