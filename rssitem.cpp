/*
    Copyright 2014-2015 Vanniktech - Niklas Baudy

    This file is part of SpeedReader.

    SpeedReader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    SpeedReader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpeedReader. If not, see <http://www.gnu.org/licenses/>.
*/

#include "rssitem.h"

#include <QVariant>

RSSItem::RSSItem(QUuid rssItemId, QString title, QString description, QString link, QString imageLocal, QString imageInternet, QDateTime date, bool speedRead) {
    mRSSItemId = rssItemId;
    mTitle = title;
    mDescription = description;
    mLink = link;
    mImageLocal = imageLocal;
    mImageInternet = imageInternet;
    mDate = date;
    mSpeedRead = speedRead;
    if (!mImageLocal.isEmpty()) mImage = QImage(mImageLocal);
}

RSSItem::RSSItem(QSqlQuery sqlQuery) {
    mRSSItemId = sqlQuery.value("rss_item_id").value<QUuid>();
    mTitle = sqlQuery.value("title").value<QString>();
    mDescription = sqlQuery.value("description").value<QString>();
    mLink = sqlQuery.value("link").value<QString>();
    mImageLocal = sqlQuery.value("image_local").value<QString>();
    mImageInternet = sqlQuery.value("image_internet").value<QString>();
    mDate = QDateTime::fromTime_t(sqlQuery.value("date").value<uint>());
    mSpeedRead = sqlQuery.value("speedread").value<bool>();
    if (!mImageLocal.isEmpty()) mImage = QImage(mImageLocal);
}

QUuid RSSItem::getRSSItemId() {
    return mRSSItemId;
}

QString RSSItem::getTitle() {
    return mTitle;
}

QString RSSItem::getDescription() {
    return mDescription;
}

QString RSSItem::getLink() {
    return mLink;
}

QString RSSItem::getImageLocal() {
    return mImageLocal;
}

QString RSSItem::getImageInternet() {
    return mImageInternet;
}

QImage RSSItem::getImage() {
    return mImage;
}

QDateTime RSSItem::getDate() {
    return mDate;
}

bool RSSItem::isSpeedRead() {
    return mSpeedRead;
}

void RSSItem::setSpeedRead(bool speedread) {
    mSpeedRead = speedread;
}

void RSSItem::flipSpeedRead() {
    mSpeedRead = !mSpeedRead;
}
