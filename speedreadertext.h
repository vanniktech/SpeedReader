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

#ifndef SPEEDREADERTEXT_H
#define SPEEDREADERTEXT_H

#include <QList>
#include <QString>
#include <QStringList>

#include "thread.h"
#include "speedreadersegment.h"
#include "speedreadertextdatasource.h"

class TextParts;

class SpeedReaderText : public Thread {
    Q_OBJECT

    public:
        enum SpeedReaderStatus { SPEEDREADING, STOPPING, FINISHED };

        SpeedReaderText(SpeedReaderTextDataSource* speedReaderTextDataSource, QObject *parent = 0);

        QString getText() const;
        int getIndex() const;
        int getMaxIndex() const;
        int getStopWordsRead() const;
        int getStopWordsCount();
        int getEstimatedTimeInMS() const;
        QString getHTMLColoredText() const;

        bool stopSpeedReading();
        void continueSpeedReading();
        void setText(const QString& text);

        static const QString HTML_ANCHOR_CURRENT_WORD;

        // Only for unit testing
        QList<SpeedReaderSegment> getDissectText() const { return mDissectText; }
    protected:
        void run();

    private:
        SpeedReaderTextDataSource* mSpeedReaderTextDataSource;
        QString mText;
        QList<SpeedReaderSegment> mDissectText;

        bool    mSpeedReading;
        int     mIndex;
        int     mStopWordsRead;
        int     mStopWordsCount;

        void dissectText();
        SpeedReaderSegment getSpeedReaderSegment(QMap<QString, int> delayWords, const QList<QString> &stopWords, const TextParts &textParts) const;
        QString prepareTextForDissecting(QString input) const;
        int getSpeedReadingTimePerMinuteInMs() const;

    signals:
        void changed(QString text, int speedreadTextInPercent, SpeedReaderText::SpeedReaderStatus status);
};

class TextParts {
public:
    static const TextParts EMPTY_TEXT_PARTS;

    TextParts() {}

    TextParts(const QString& string) {
        this->add(string);
    }

    void add(const QString& string) {
        mParts << string;
    }

    int size() const {
        return mParts.size();
    }

    bool contains(const QString& word) const {
        return mParts.contains(word);
    }

    bool contains(const QList<QString>& words) const {
        for (const QString word : words) {
            if (this->contains(word)) {
                return true;
            }
        }

        return false;
    }

    int numberOfOccurrences(const QString& word) const {
        return mParts.count(word);
    }

    void clear() {
        mParts.clear();
    }

    QString toString() const {
        QString result;

        for (QString part : mParts) {
            result.append(part).append(' '); // TODO make constant
        }

        return result.trimmed();
    }

private:
    QStringList mParts;
};

#endif // SPEEDREADERTEXT_H
