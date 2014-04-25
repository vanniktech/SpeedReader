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

#ifndef SPEEDREADERTEXT_H
#define SPEEDREADERTEXT_H

#include <QList>
#include <QString>
#include <QThread>

class SpeedReaderText : public QThread {
    Q_OBJECT

    public:
        enum SpeedReaderStatus {
            READING = 0,
            STOPPING = 1,
            FINISHED = 2
        };

        SpeedReaderText(QString text);
        QString getText();
        QString getRemainingText();

        bool isReading();
        int getIndex();
        int getMaxIndex();
        int getStopWordsRead();
        int getStopWordsCount();
        int getEstimatedTimeInMS();
        QString getHTMLColoredText();

        void stopReading();
        void continueReading();
        void resetReading();

    protected:
        void run();

    private:
        QString mText;
        QList<QString> mDissectText;

        int     mIndex;
        int     mStopWordsRead;
        bool    mReading;
        int     mStopWordsCount;

        void dissectText();

    signals:
        void changed(QString text, int readTextInPercent, SpeedReaderText::SpeedReaderStatus status);
};

#endif // SPEEDREADERTEXT_H
