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

#include "vntrsshandler.h"

#include <QMap>

VNTRSSHandler::VNTRSSHandler() {
}

VNTRSSHandler::~VNTRSSHandler() {
}

const QString VNTRSSHandler::getItemName() const {
    return "item";
}

const QMap<QString, QString> VNTRSSHandler::getRSSChannelMapping() const {
    QMap<QString, QString> mapping;
    mapping.insert("url", "imageUrl");
    return mapping;
}

const QMap<QString, QString> VNTRSSHandler::getRSSItemMapping() const {
    return QMap<QString, QString>();
}

const QList<VNTProtocolSpecialCase> VNTRSSHandler::getRSSChannelSpecialCases() const {
    QList<VNTProtocolSpecialCase> specialCases;
    return specialCases;
}

const QList<VNTProtocolSpecialCase> VNTRSSHandler::getRSSItemSpecialCases() const {
    QList<VNTProtocolSpecialCase> specialCases;
    specialCases.append(VNTProtocolSpecialCaseBuilder().setTag("media:thumbnail").setValueName("url").setName("imageUrl").build());
    specialCases.append(VNTProtocolSpecialCaseBuilder().setTag("media:content").setValueName("url").setName("imageUrl").build());
    return specialCases;
}

const QList<QStack<QString> > VNTRSSHandler::getRSSChannelIgnoredTags() const {
    QList<QStack<QString> > ignoredTags;

    QStack<QString> imageTitleTag;
    imageTitleTag.push("image");
    imageTitleTag.push("title");
    ignoredTags.append(imageTitleTag);

    QStack<QString> imageLinkTag;
    imageLinkTag.push("image");
    imageLinkTag.push("link");
    ignoredTags.append(imageLinkTag);

    return ignoredTags;
}

const QList<QStack<QString> > VNTRSSHandler::getRSSItemIgnoredTags() const {
    QList<QStack<QString> > ignoredTags;

    QStack<QString> imageTitleTag;
    imageTitleTag.push("media:content");
    imageTitleTag.push("media:title");
    ignoredTags.append(imageTitleTag);

    return ignoredTags;
}
