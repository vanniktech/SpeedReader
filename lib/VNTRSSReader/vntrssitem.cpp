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

#include <QRegExp>

#include "vntrssitem.h"

VNTRSSItem::VNTRSSItem() : VNTRSSCommon() {

}

void VNTRSSItem::setDescription(const QString &description) {
    VNTRSSCommon::setDescription(description);

    if (mImageUrl.isEmpty()) {
        QRegExp imageRegex("src=\"?(http://|https://)(www)?[a-zA-Z0-9~\\+\\$\\=\\%\\^\\&\\!\\-\\#\\_\\?./]+(\\.jpg|\\.JPG|\\.png|\\.PNG|\\.jpeg|\\.JPEG)\"?");

        if (imageRegex.indexIn(description) != -1) {
            this->setImageUrl(imageRegex.cap().remove("src=").remove('"'));
        }
    }
}

void VNTRSSItem::setGuid(const QString &guid) {
    mGuid = guid.simplified();
}

QString VNTRSSItem::getGuid() const {
    return mGuid;
}

void VNTRSSItem::setCategory(const QString &category) {
    mCategory = category.simplified();
}

QString VNTRSSItem::getCategory() const {
    return mCategory;
}

QString VNTRSSItem::toString() const {
    return VNTRSSCommon::toString().append(QString("\ncategory=%1\nguid=%2").arg(mCategory, mGuid));
}
