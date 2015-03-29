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
#include <QStringList>

#include "vntrsscommon.h"

VNTRSSCommon::VNTRSSCommon() : QObject(0) {

}

void VNTRSSCommon::setTitle(const QString &title) {
    mTitle = title;
    mPlainTitle = QString(title).remove(QRegExp("<[^>]*>")).replace('\n', ' ').replace(QRegExp("([\\s])+"), "\\1").trimmed();
}

QString VNTRSSCommon::getTitle() const {
    return mTitle;
}

QString VNTRSSCommon::getPlainTitle() const {
    return mPlainTitle;
}

void VNTRSSCommon::setDescription(const QString &description) {
    mDescription = description;
    mPlainDescription = QString(description).remove(QRegExp("<[^>]*>")).replace(QRegExp("([\\s])+"), "\\1").trimmed();
}

QString VNTRSSCommon::getDescription() const {
    return mDescription;
}

QString VNTRSSCommon::getPlainDescription() const {
    return mPlainDescription;
}

void VNTRSSCommon::setPubDate(const QString &pubDate) {
    QString pubDateFormatted = pubDate.simplified();

    // Atom
    if (pubDateFormatted.contains(QRegExp("[0-9]{4}\\-[0-9]{2}\\-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}"))) {
        mPubDate = QDateTime::fromString(pubDateFormatted.mid(0, 19), "yyyy-MM-ddTHH:mm:ss");
    } else { // RSS
        pubDateFormatted = pubDateFormatted.mid(5).mid(0, 20); // replace weekday (e.g.:Thu, ) and timezone (e.g.: +0200, GMT), which is at the end of the string

        // need to do this, because month have different abbreviations on different operating systems with different languages
        pubDateFormatted = pubDateFormatted.replace("Jan", "01"); pubDateFormatted = pubDateFormatted.replace("Feb", "02"); pubDateFormatted = pubDateFormatted.replace("Mar", "03"); pubDateFormatted = pubDateFormatted.replace("Apr", "04"); pubDateFormatted = pubDateFormatted.replace("May", "05"); pubDateFormatted = pubDateFormatted.replace("Jun", "06"); pubDateFormatted = pubDateFormatted.replace("Jul", "07"); pubDateFormatted = pubDateFormatted.replace("Aug", "08"); pubDateFormatted = pubDateFormatted.replace("Sep", "09"); pubDateFormatted = pubDateFormatted.replace("Oct", "10"); pubDateFormatted = pubDateFormatted.replace("Nov", "11"); pubDateFormatted = pubDateFormatted.replace("Dez", "12");

        // TODO check MMM
        mPubDate = QDateTime::fromString(pubDateFormatted, "dd MM yyyy HH:mm:ss");
    }
}

QDateTime VNTRSSCommon::getDate() const {
    return mPubDate;
}

void VNTRSSCommon::setLink(const QString &link) {
    mLink = QUrl(link);
}

QUrl VNTRSSCommon::getLink() const {
    return mLink;
}

void VNTRSSCommon::setImageUrl(const QString &imageUrl) {
    mImageUrl = QUrl::fromUserInput(imageUrl);
    this->updateImageFileType();
}

QUrl VNTRSSCommon::getImageUrl() const {
    return mImageUrl;
}

QString VNTRSSCommon::getImageFileType() const {
    return mImageFileType;
}

QImage VNTRSSCommon::getImage() const {
    return mImage;
}

void VNTRSSCommon::setImage(const QImage &value) {
    mImage = value;
}

QString VNTRSSCommon::toString() const {
    return QString("link=%1\ntitle=%2\ndescription=%3\npubDate=%4\nimageUrl=%5").arg(mLink.toString(), mTitle, mDescription, mPubDate.toString(), mImageUrl.toString());
}

void VNTRSSCommon::updateImageFileType() {
    mImageFileType = mImageUrl.toString().split(".").last();
}
