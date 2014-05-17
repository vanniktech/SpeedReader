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

#include <QRegExp>

#include "vntrsscommon.h"

VNTRSSCommon::VNTRSSCommon(QString title, QString description, QString pubDate, QUrl link, QUrl imageUrl) {
    mTitle = title.simplified();
    mDescription = description.simplified();
    mPubDate = pubDate.simplified();
    mLink = link;
    mImageUrl = imageUrl;
}

QString VNTRSSCommon::getTitle() const {
    return mTitle;
}

QString VNTRSSCommon::getDescription() const {
    return mDescription;
}

QString VNTRSSCommon::getPlainDescription() {
    return mDescription.remove(QRegExp("<[^>]*>")).simplified();
}

QString VNTRSSCommon::getPubDate() const {
    return mPubDate;
}

QUrl VNTRSSCommon::getLink() const {
    return mLink;
}

QUrl VNTRSSCommon::getImageUrl() const {
    return mImageUrl;
}

QImage VNTRSSCommon::getImage() const {
    return mImage;
}

void VNTRSSCommon::setImage(const QImage &value) {
    mImage = value;
}

QString VNTRSSCommon::toString() const {
    return QString("link=%1\ntitle=%2\ndescription=%3\npubDate=%4\nimageUrl=%5").arg(mLink.toString(), mTitle, mDescription, mPubDate, mImageUrl.toString());
}
