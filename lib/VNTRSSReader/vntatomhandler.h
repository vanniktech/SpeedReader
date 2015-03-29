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

#ifndef VNTATOMHANDLER_H
#define VNTATOMHANDLER_H

#include "vntprotocolhandler.h"

class VNTAtomHandler : public VNTProtocolHandler
{
public:
    VNTAtomHandler();
    ~VNTAtomHandler();

    const QString getItemName() const;

    VNTRSSChannel *parseRSSChannel(QXmlStreamReader& xml);

protected:
    const QMap<QString, QString> getRSSChannelMapping() const;
    const QMap<QString, QString> getRSSItemMapping() const;

    const QList<VNTProtocolSpecialCase> getRSSChannelSpecialCases() const;
    const QList<VNTProtocolSpecialCase> getRSSItemSpecialCases() const;

    const QList<QStack<QString> > getRSSChannelIgnoredTags() const;
    const QList<QStack<QString> > getRSSItemIgnoredTags() const;
};

#endif // VNTATOMHANDLER_H
