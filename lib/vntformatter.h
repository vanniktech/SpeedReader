/*
    Copyright 2014 Vanniktech - Niklas Baudy

    This file is part of SpeedReader.

    SpeedReader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    SpeedReader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpeedReader. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VNTFORMATTER_H
#define VNTFORMATTER_H

#include <QString>
#include "qmath.h"

class VNTFormatter
{
    public:
        static QString getFormattedTimePart(int value) {
            QString s = QString::number(value);

            if (value < 10) s.insert(0, '0');

            return s;
        }

        static QString getFormattedMSPart(int ms) {
            QString s = QString::number(ms);

            if (ms < 100) s.insert(0, '0');
            if (ms < 10) s.insert(0, '0');

            return s;
        }

        static QString formatMilliseconds(int millis) {
            int h = floor(millis / 1000 / 60 / 60);
            int m = floor(millis / 1000 / 60) - h * 60;
            int s = floor((millis / 1000) % 60);
            int ms = millis - (h * 1000 * 60 * 60) - (m * 1000 * 60) - (s * 1000);

            return QString("%1:%2:%3:%4").arg(VNTFormatter::getFormattedTimePart(h), VNTFormatter::getFormattedTimePart(m), VNTFormatter::getFormattedTimePart(s), VNTFormatter::getFormattedMSPart(ms));
        }
};

#endif // VNTFORMATTER_H
