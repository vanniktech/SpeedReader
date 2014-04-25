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

#include "vntrssitem.h"

#include <QRegExp>

VNTRSSItem::VNTRSSItem(QString link, QString title, QString description, QString pubDate, QString category, QString guid, QString imageUrl) {
    mLink = link.trimmed();
    mTitle = title.simplified();
    mDescription = description.simplified();
    mPubDate = pubDate.trimmed();
    mCategory = category.simplified();
    mGuid = guid;

    if (imageUrl.isNull()) {
        QRegExp imageRegex("src=\"(http://|https://)(www)?[a-zA-Z0-9\\+\\$\\=\\%\\^\\&\\!\\-\\#\\_\\?./]+\"");

        if (imageRegex.indexIn(mDescription) != -1) imageUrl = imageRegex.cap().remove("src=\"").remove('"');
    }

    mImageUrl = QUrl(imageUrl);
}

VNTRSSItem::~VNTRSSItem() {
}

QString VNTRSSItem::getTitle() {
    return mTitle;
}

QString VNTRSSItem::getDescription() {
    return mDescription;
}

QString VNTRSSItem::getPlainDescription() {
    return mDescription.remove(QRegExp("<[^>]*>")).simplified();
}

QString VNTRSSItem::getLink() {
    return mLink;
}

QString VNTRSSItem::getGuid() {
    return mGuid;
}

QString VNTRSSItem::getCategory() {
    return mCategory;
}

QString VNTRSSItem::getPubDate() {
    return mPubDate;
}

QUrl    VNTRSSItem::getImageUrl() {
    return mImageUrl;
}

QString VNTRSSItem::toString() {
    return QString("link=%1\ntitle=%2\ndescription=%3\npubDate=%4\ncategory=%5\nguid=%6\nimageUrl=%7").arg(mLink, mTitle, mDescription, mPubDate, mCategory, mGuid, mImageUrl.toString());
}
