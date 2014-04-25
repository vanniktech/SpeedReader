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

#include "vntrsschannel.h"

VNTRSSChannel::VNTRSSChannel(QString link, QString title, QString description, QString pubdate, QString language, QString copyright, QString imageUrl, QUrl rssUrl, QList<VNTRSSItem> items) {
    mLink = QUrl(link);
    mTitle = title.simplified();
    mDescription = description.simplified();
    mPubDate = pubdate;
    mLanguage = language;
    mCopyright = copyright.simplified();
    mImageUrl = QUrl(imageUrl);
    mRSSUrl = rssUrl;
    mItems = items;
}

VNTRSSChannel::~VNTRSSChannel() {
}

QString VNTRSSChannel::getTitle() {
    return mTitle;
}

QString VNTRSSChannel::getDescription() {
    return mDescription;
}

QUrl VNTRSSChannel::getLink() {
    return mLink;
}

QString VNTRSSChannel::getPubDate() {
    return mPubDate;
}

QString VNTRSSChannel::getLanguage() {
    return mLanguage;
}

QString VNTRSSChannel::getCopyright() {
    return mCopyright;
}

QUrl    VNTRSSChannel::getImageUrl() {
    return mImageUrl;
}

QUrl    VNTRSSChannel::getRSSUrl() {
    return mRSSUrl;
}

QList<VNTRSSItem> VNTRSSChannel::getItems() {
    return mItems;
}

QString VNTRSSChannel::toString() {
    return QString("link=%1\ntitle=%2\ndescription=%3\npubDate=%4\nlanguage=%5\ncopyright=%6\nrssUrl=%7").arg(mLink.toString(), mTitle, mDescription, mPubDate, mLanguage, mCopyright, mRSSUrl.toString());
}
