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

#include "vntatomhandler.h"

VNTAtomHandler::VNTAtomHandler() {
}

VNTAtomHandler::~VNTAtomHandler() {
}

const QString VNTAtomHandler::getItemName() const {
    return "entry";
}

const QMap<QString, QString> VNTAtomHandler::getRSSChannelMapping() const {
    QMap<QString, QString> mapping;
    mapping.insert("subtitle", "description");
    mapping.insert("updated", "pubDate");
    mapping.insert("logo", "imageUrl");
    mapping.insert("published", "pubDate");
    return mapping;
}

const QMap<QString, QString> VNTAtomHandler::getRSSItemMapping() const {
    QMap<QString, QString> mapping;
    mapping.insert("summary", "description");
    mapping.insert("updated", "pubDate");
    mapping.insert("logo", "imageUrl");
    mapping.insert("published", "pubDate");
    return mapping;
}

const QList<VNTProtocolSpecialCase> VNTAtomHandler::getRSSChannelSpecialCases() const {
    QList<VNTProtocolSpecialCase> specialCases;
    specialCases.append(VNTProtocolSpecialCaseBuilder().setTag("link").setKeyName("rel").setKeyValue("alternate").setValueName("href").setName("link").build());
    return specialCases;
}

const QList<VNTProtocolSpecialCase> VNTAtomHandler::getRSSItemSpecialCases() const {
    QList<VNTProtocolSpecialCase> specialCases;
    specialCases.append(VNTProtocolSpecialCaseBuilder().setTag("link").setKeyName("rel").setKeyValue("alternate").setValueName("href").setName("link").build());
    specialCases.append(VNTProtocolSpecialCaseBuilder().setTag("link").setKeyName("type").setKeyValue("image/jpeg").setValueName("href").setName("imageUrl").build());
    specialCases.append(VNTProtocolSpecialCaseBuilder().setTag("link").setKeyName("type").setKeyValue("image/png").setValueName("href").setName("imageUrl").build());
    return specialCases;
}

const QList<QStack<QString> > VNTAtomHandler::getRSSChannelIgnoredTags() const {
    return QList<QStack<QString> >();
}

const QList<QStack<QString> > VNTAtomHandler::getRSSItemIgnoredTags() const {
    return QList<QStack<QString> >();
}

VNTRSSChannel *VNTAtomHandler::parseRSSChannel(QXmlStreamReader& xml) {
    QString language = xml.attributes().value("xml:lang").toString();

    VNTRSSChannel* rssChannel = VNTProtocolHandler::parseRSSChannel(xml);
    rssChannel->setLanguage(language);

    return rssChannel;
}
