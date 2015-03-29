/*
    Copyright 2014-2015 Vanniktech - Niklas Baudy

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

#include <QObject>
#include <QString>
#include <QImage>
#include <QUrl>
#include <QDateTime>

class VNTRSSCommon : public QObject {
    Q_OBJECT

public:
    VNTRSSCommon();

    Q_INVOKABLE void setTitle(const QString &title);
    QString getTitle() const;
    QString getPlainTitle() const;

    Q_INVOKABLE void setDescription(const QString &description);
    QString getDescription() const;
    QString getPlainDescription() const;

    Q_INVOKABLE void setPubDate(const QString &pubDate);
    QDateTime getDate() const;

    Q_INVOKABLE void setLink(const QString &link);
    QUrl getLink() const;

    Q_INVOKABLE void setImageUrl(const QString &imageUrl);
    QUrl getImageUrl() const;

    QString getImageFileType() const;
    QImage getImage() const;
    void setImage(const QImage &value);

    QString toString() const;

protected:
    QString mTitle;
    QString mPlainTitle;
    QString mDescription;
    QString mPlainDescription;
    QDateTime mPubDate;
    QUrl    mLink;
    QUrl    mImageUrl;
    QString mImageFileType;
    QImage  mImage;

    void updateImageFileType();
};

#endif // VNTRSSCOMMON_H
