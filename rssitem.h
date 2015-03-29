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

#ifndef RSSITEM_H
#define RSSITEM_H

#include <QString>
#include <QDateTime>
#include <QUrl>
#include <QUuid>
#include <QSqlQuery>
#include <QImage>

class RSSItem {
public:
    RSSItem(QUuid rssItemId, QString title, QString description, QString link, QString imageLocal, QString imageInternet, QDateTime date, bool speedRead);
    RSSItem(QSqlQuery sqlQuery);

    QUuid getRSSItemId();
    QString getTitle();
    QString getDescription();
    QString getLink();
    QString getImageLocal();
    QString getImageInternet();
    QDateTime getDate();
    QImage getImage();
    bool isSpeedRead();
    void setSpeedRead(bool speedread);
    void flipSpeedRead();

private:
    QUuid mRSSItemId;
    QString mTitle;
    QString mDescription;
    QString mLink;
    QString mImageLocal;
    QString mImageInternet;
    QDateTime mDate;
    QImage  mImage;
    bool mSpeedRead;
};

#endif // RSSITEM_H
