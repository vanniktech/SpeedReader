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

#include "vntrssitem.h"

VNTRSSItem::VNTRSSItem(QString link, QString title, QString description, QString pubDate, QString category, QString guid, QString imageUrl) : VNTRSSCommon(title, description, pubDate, link, QUrl()){
    mCategory = category.simplified();
    mGuid = guid.simplified();

    if (imageUrl.isNull()) {
        QRegExp imageRegex("src=\"(http://|https://)(www)?[a-zA-Z0-9\\+\\$\\=\\%\\^\\&\\!\\-\\#\\_\\?./]+\"");

        if (imageRegex.indexIn(mDescription) != -1) imageUrl = imageRegex.cap().remove("src=\"").remove('"');
    }

    mImageUrl = QUrl(imageUrl);
}

QString VNTRSSItem::getGuid() const {
    return mGuid;
}

QString VNTRSSItem::getCategory() const {
    return mCategory;
}

QString VNTRSSItem::toString() const {
    return VNTRSSCommon::toString().append(QString("category=%1\nguid=%2").arg(mCategory, mGuid));
}
