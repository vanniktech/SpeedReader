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

#include "vntprotocolhandler.h"

VNTRSSChannel* VNTProtocolHandler::parseRSSChannel(QXmlStreamReader &xml) {
    mKeyValueMap.clear();

    const QList<VNTProtocolSpecialCase> specialCases = this->getRSSChannelSpecialCases();
    const QList<QStack<QString> > ignoredTags = this->getRSSChannelIgnoredTags();

    QStack<QString> xmlTags;

    while (!xml.atEnd() && xml.name() != this->getItemName()) {
        this->readXml(xml, xmlTags, specialCases, ignoredTags);
    }

    VNTRSSChannel* rssChannel = new VNTRSSChannel();
    this->setObjectValues(rssChannel, this->getRSSChannelMapping());

    return rssChannel;
}

VNTRSSItem* VNTProtocolHandler::parseRSSItem(QXmlStreamReader& xml) {
    mKeyValueMap.clear();

    const QList<VNTProtocolSpecialCase> specialCases = this->getRSSItemSpecialCases();
    const QList<QStack<QString> > ignoredTags = this->getRSSItemIgnoredTags();

    QStack<QString> xmlTags;

    xml.readNextStartElement();

    while(!(xml.isEndElement() && xml.name() == this->getItemName()) && !xml.atEnd()) {
        this->readXml(xml, xmlTags, specialCases, ignoredTags);
    }

    VNTRSSItem* rssItem = new VNTRSSItem();
    this->setObjectValues(rssItem, this->getRSSItemMapping());

    return rssItem;
}

void VNTProtocolHandler::setObjectValues(QObject* object, const QMap<QString, QString> &mapping) {
    QMapIterator<QString, QString> it(mKeyValueMap);

    while (it.hasNext()) {
        it.next();

        const QString key = it.key();
        const QString value = it.value();

        const QString property = mapping.contains(key) ? mapping.value(key) : key;

        const QString functionName = QString ("set%1%2").arg(property.at(0).toUpper(), property.mid(1));
        const QString functionNameWithArgument = QString(functionName).append("(QString)");

        const QMetaObject* metaObject = object->metaObject();
        const int functionIndex = metaObject->indexOfMethod(QMetaObject::normalizedSignature(functionNameWithArgument.toStdString().c_str()));

        if (functionIndex >= 0) {
            QMetaObject::invokeMethod(object, functionName.toStdString().c_str(), Qt::DirectConnection, Q_ARG(QString, value));
        }
    }
}

void VNTProtocolHandler::handleSpecialCasesInKeyValueMap(const QList<VNTProtocolSpecialCase> &specialCases, const QXmlStreamAttributes &attributes, const QString &key) {
    foreach (const VNTProtocolSpecialCase specialCase, specialCases) {
        if (key == specialCase.tag && (specialCase.keyName.isEmpty() || attributes.value(specialCase.keyName) == specialCase.keyValue)) {
            mKeyValueMap.insert(specialCase.name, attributes.value(specialCase.valueName).toString());
        }
    }
}

bool VNTProtocolHandler::shouldIgnoreXmlTag(const QString &xmlTag, QStack<QString> xmlTags, const QList<QStack<QString> > ignoredTags) {
    foreach (QStack<QString> ignoredTag, ignoredTags) {
        if (xmlTag == ignoredTag.top()) {
            bool shouldBreak = false;

            while (ignoredTags.size() <= xmlTags.size() && !ignoredTag.isEmpty() && !xmlTags.isEmpty()) {
                shouldBreak = ignoredTag.pop() == xmlTags.pop();
            }

            if (shouldBreak) {
                return true;
            }
        }
    }

    return false;
}

void VNTProtocolHandler::readXml(QXmlStreamReader &xml, QStack<QString> &xmlTags, const QList<VNTProtocolSpecialCase> &specialCases, const QList<QStack<QString> > &ignoredTags) {
    const QString prefix = xml.prefix().toString();
    const QString key = xml.name().toString();
    const QString xmlTag = prefix.isEmpty() ? key : prefix + ":" + key;

    if (xml.isStartElement()) {
        this->handleSpecialCasesInKeyValueMap(specialCases, xml.attributes(), xmlTag);

        xml.readNext();

        if (xml.isCharacters() && key != this->getItemName()) {
            xmlTags.append(xmlTag);

            if (this->shouldIgnoreXmlTag(xmlTag, xmlTags, ignoredTags)) {
                return;
            }

            mKeyValueMap.insert(key, xml.text().toString());
        }

        this->popXmlTagIfPossible(xmlTag, xmlTags, xml);
    }

    this->popXmlTagIfPossible(xmlTag, xmlTags, xml);

    xml.readNextStartElement();
}

void VNTProtocolHandler::popXmlTagIfPossible(const QString &xmlTag, QStack<QString> &xmlTags, QXmlStreamReader &xml) {
    if (xml.isEndElement() && !xmlTags.isEmpty() && xmlTags.top() == xmlTag) {
        xmlTags.pop();
    }
}
