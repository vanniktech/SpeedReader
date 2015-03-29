/*
    Copyright 2014-2015 Vanniktech - Niklas Baudy

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

#ifndef SPEEDREADERTEXTDATASOURCE_H
#define SPEEDREADERTEXTDATASOURCE_H

#include <QList>
#include <QMap>

class SpeedReaderTextDataSource
{
    public:
        virtual ~SpeedReaderTextDataSource() {}
        virtual bool shouldStallAtIndentions() = 0;
        virtual int getNumberOfWords() = 0;
        virtual int getWordsPerMinute() = 0;
        virtual int getWordLength() = 0;
        virtual QList<QString> getBreakWords() = 0;
        virtual QList<QString> getStopWords() = 0;
        virtual QMap<QString, int> getDelayWords() = 0;
        virtual bool shouldGroupNumbers() = 0;
        virtual bool shouldDisplayLongerWordsLonger() = 0;
};

#endif // SPEEDREADERTEXTDATASOURCE_H
