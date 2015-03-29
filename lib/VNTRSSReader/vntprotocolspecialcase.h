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

#ifndef VNTPROTOCOLSPECIALCASE_H
#define VNTPROTOCOLSPECIALCASE_H

#include <QString>

// <link rel="alternate" href="http://vanniktech.de" /> would be
// tag          = link
// keyName      = rel
// keyValue     = alternate
// valueName    = href
// name         = url (setUrl would be called then)
struct VNTProtocolSpecialCase {
    QString tag;
    QString keyName;
    QString keyValue;
    QString valueName;
    QString name;
};

class VNTProtocolSpecialCaseBuilder {
public:
    VNTProtocolSpecialCaseBuilder() {
        mTag = mKeyName = mKeyValue = mValueName = mName = "";
    }

    VNTProtocolSpecialCaseBuilder& setTag(const QString &tag) { mTag = tag; return *this; }
    VNTProtocolSpecialCaseBuilder& setKeyName(const QString &keyName) { mKeyName = keyName; return *this; }
    VNTProtocolSpecialCaseBuilder& setKeyValue(const QString &keyValue) { mKeyValue = keyValue; return *this; }
    VNTProtocolSpecialCaseBuilder& setValueName(const QString &valueName) { mValueName = valueName; return *this; }
    VNTProtocolSpecialCaseBuilder& setName(const QString &name) { mName = name; return *this; }

    const VNTProtocolSpecialCase build() const {
        VNTProtocolSpecialCase protocolSpecialCase;
        protocolSpecialCase.tag = mTag;
        protocolSpecialCase.keyName = mKeyName;
        protocolSpecialCase.keyValue = mKeyValue;
        protocolSpecialCase.valueName = mValueName;
        protocolSpecialCase.name = mName;
        return protocolSpecialCase;
    }

private:
    QString mTag;
    QString mKeyName;
    QString mKeyValue;
    QString mValueName;
    QString mName;
};

#endif // VNTPROTOCOLSPECIALCASE_H
