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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include <QString>
#include <QColor>
#include <QUrl>
#include "word.h"

class Settings : public QObject {
        Q_OBJECT

    signals:
        void updatedSettings();

    public:
        static Settings* getInstance();

        QList<QString> dissectText(QString text);
        int getReadingTimePerMinuteInMs();

        void synchronize();

        QColor getTextColor();
        void setTextColor(QColor textColor);
        QColor getTextBackgroundColor();
        void setTextBackgroundColor(QColor textBackgroundColor);
        QString getFontFamily();
        void setFontFamily(QString fontFamily);
        int getFontSize();
        void setFontSize(int fontSize);
        int getNumberOfWords();
        void setNumberOfWords(int numberOfWords);
        bool changedNumberOfWords();
        int getWordsPerMinute();
        void setWordsPerMinute(int wordsPerMinute);
        bool changedWordsPerMinute();
        bool isNumberGrouping();
        void setNumberGrouping(bool numberGrouping);
        bool changedNumberGrouping();
        bool isJumpBackToStart();
        void setJumpBackToStart(bool jumpBackToStart);

        QList<Word> getWords();
        QList<QString> getStopWords();
        QList<QString> getBreakWords();
        void setWords(QList<Word> stopWords);
        bool changedWords();

        QList<QUrl> getRSSUrls();
        void setRSSUrls(QList<QUrl> rssUrls);
        bool changedRSSUrls();
        bool appendRSSUrl(QUrl rssUrl);

    private:
        Settings();
        Settings(Settings const&);
        Settings& operator=(Settings const&);
        static Settings* mInstance;

        QColor  mTextColor;
        QColor  mTextBackgroundColor;
        QString mFontFamily;
        int mFontSize;
        int mNumberOfWords;
        bool mChangedNumberOfWords;
        int mWordsPerMinute;
        bool mChangedWordsPerMinute;
        bool mNumberGrouping;
        bool mChangedNumberGrouping;
        bool mJumpBackToStart;
        QList<Word> mWords;
        bool mChangedWords;
        QList<QString> mStopWords;
        QList<QString> mBreakWords;
        QList<QUrl> mRSSUrls;
        bool mChangedRSSUrls;

        void appendStopWord(QString value, bool stopWord, bool breakWord);
};

#endif // SETTINGS_H
