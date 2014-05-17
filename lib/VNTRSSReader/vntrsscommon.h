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

#ifndef VNTRSSCOMMON_H
#define VNTRSSCOMMON_H

#include <QString>
#include <QImage>
#include <QUrl>

class VNTRSSCommon {
public:
    VNTRSSCommon(QString title, QString description, QString pubDate, QUrl link, QUrl imageUrl);

    QString getTitle() const;
    QString getDescription() const;
    QString getPlainDescription();
    QString getPubDate() const;
    QUrl getLink() const;
    QUrl getImageUrl() const;
    QImage getImage() const;
    void setImage(const QImage &value);

    QString toString() const;

protected:
    QString mTitle;
    QString mDescription;
    QString mPubDate;
    QUrl    mLink;
    QUrl    mImageUrl;
    QImage  mImage;
};

#endif // VNTRSSCOMMON_H
