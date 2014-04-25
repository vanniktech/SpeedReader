/*
    Copyright 2014 Vanniktech - Niklas Baudy

    This file is part of VNTRSSReader.

    VNTRSSReader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    VNTRSSReader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VNTRSSReader. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VNTRSSCHANNEL_H
#define VNTRSSCHANNEL_H

#include <QString>
#include <QList>
#include <QUrl>

#include <lib/VNTRSSReader/vntrssitem.h>

class VNTRSSChannel
{
public:
    VNTRSSChannel(QString link, QString title, QString description, QString pubdate, QString language, QString copyright, QString imageUrl, QUrl rssUrl, QList<VNTRSSItem> items);
    ~VNTRSSChannel();

    QString getTitle();
    QString getDescription();
    QUrl     getLink();
    QString getPubDate();
    QString getLanguage();
    QString getCopyright();
    QUrl    getImageUrl();
    QUrl    getRSSUrl();

    QList<VNTRSSItem> getItems();

    QString toString();

private:
    QUrl    mLink;
    QString mTitle;
    QString mDescription;
    QString mPubDate;
    QString mLanguage;
    QString mCopyright;
    QUrl    mImageUrl;
    QUrl    mRSSUrl;
    QList<VNTRSSItem> mItems;
};

#endif // VNTRSSCHANNEL_H
