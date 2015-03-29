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

#ifndef VNTPROTOCOLHANDLER_H
#define VNTPROTOCOLHANDLER_H

#include <QMap>
#include <QStack>
#include <QXmlStreamReader>

#include "vntrsschannel.h"
#include "vntrssitem.h"
#include "vntprotocolspecialcase.h"

class VNTProtocolHandler {
public:
    virtual ~VNTProtocolHandler() {}

    virtual const QString getItemName() const = 0;

    virtual VNTRSSChannel* parseRSSChannel(QXmlStreamReader& xml);
    VNTRSSItem* parseRSSItem(QXmlStreamReader& xml);

protected:
    virtual const QMap<QString, QString> getRSSChannelMapping() const = 0;
    virtual const QMap<QString, QString> getRSSItemMapping() const = 0;

    virtual const QList<VNTProtocolSpecialCase> getRSSChannelSpecialCases() const = 0;
    virtual const QList<VNTProtocolSpecialCase> getRSSItemSpecialCases() const = 0;

    virtual const QList<QStack<QString> > getRSSChannelIgnoredTags() const = 0;
    virtual const QList<QStack<QString> > getRSSItemIgnoredTags() const = 0;

private:
    QMap<QString, QString> mKeyValueMap;

    void readXml(QXmlStreamReader &xml, QStack<QString> &xmlTags, const QList<VNTProtocolSpecialCase> &specialCases, const QList<QStack<QString> > &ignoredTags);
    void setObjectValues(QObject *object, const QMap<QString, QString> &mapping);
    void handleSpecialCasesInKeyValueMap(const QList<VNTProtocolSpecialCase> &specialCases, const QXmlStreamAttributes &attributes, const QString &key);
    void popXmlTagIfPossible(const QString &xmlTag, QStack<QString> &xmlTags, QXmlStreamReader &xml);

    bool shouldIgnoreXmlTag(const QString &xmlTag, QStack<QString> xmlTags, const QList<QStack<QString> > ignoredTags);
};


#endif // VNTPROTOCOLHANDLER_H
