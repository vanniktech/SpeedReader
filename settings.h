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
#include <QNetworkProxy>
#include <QMap>

#include "word.h"
#include "speedreadersegment.h"

class Settings : public QObject {
        Q_OBJECT

    signals:
        void updatedSettings();

    public:
        static const int NO_HTTP_NETWORK_PROXY = 0;
        static const int USE_SYSTEM_HTTP_NETWORK_PROXY_CONFIGURATION = 1;
        static const int CUSTOM_HTTP_NETWORK_PROXY = 2;

        static const int DOUBLE_CLICK_RSS_FEED_OPEN_INTERNALLY = 0;
        static const int DOUBLE_CLICK_RSS_FEED_OPEN_EXTERNALLY = 1;

        static Settings* getInstance();

        QList<SpeedReaderSegment> dissectText(QString text);
        int getReadingTimePerMinuteInMs();

        void synchronize();

        QColor getTextColor();
        void setTextColor(QColor textColor);
        QColor getTextBackgroundColor();
        void setTextBackgroundColor(QColor textBackgroundColor);
        QString getFontFamily();
        void setFontFamily(QString fontFamily);
        bool displayLongerWordsLonger();
        void setDisplayLongerWordsLonger(bool displayLongerWordsLonger);
        int getWordLength();
        void setWordLength(int wordLength);
        bool changedDisplayLongerWordsLonger();
        int getFontSize();
        void setFontSize(int fontSize);
        int getNumberOfWords();
        void setNumberOfWords(int numberOfWords);
        bool changedNumberOfWords();
        int getWordsPerMinute();
        void setWordsPerMinute(int wordsPerMinute);
        bool changedWordsPerMinute();
        bool numberGrouping();
        void setNumberGrouping(bool numberGrouping);
        bool changedNumberGrouping();
        bool jumpBackToStart();
        void setJumpBackToStart(bool jumpBackToStart);
        bool changedStallAtIndentions();
        bool stallAtIndentions();
        void setStallAtIndentions(bool stallAtIndentions);
        bool changedHTTPNetworkProxy();
        int getHTTPNetworkProxyType();
        void setHTTPNetworkProxyType(int httpNetworkProxyType);
        QNetworkProxy getHTTPNetworkProxy();
        void setHTTPNetworkProxy(QNetworkProxy httpNetworkProxy);

        QList<Word> getWords();
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
        bool mDisplayLongerWordsLonger;
        int mWordLength;
        bool mChangedDisplayLongerWordsLonger;
        int mNumberOfWords;
        bool mChangedNumberOfWords;
        int mWordsPerMinute;
        bool mChangedWordsPerMinute;
        bool mNumberGrouping;
        bool mChangedNumberGrouping;
        bool mJumpBackToStart;
        bool mChangedStallAtIndentions;
        bool mStallAtIndentions;
        int mHTTPNetworkProxyType;
        QNetworkProxy mHTTPNetworkProxy;
        bool mChangedHTTPNetworkProxy;

        QList<Word> mWords;
        bool mChangedWords;
        QList<QString> mStopWords;
        QList<QString> mBreakWords;
        QMap<QString, int> mDelayWords;
        QList<QUrl> mRSSUrls;
        bool mChangedRSSUrls;

        void appendWord(QString value, bool stopWord, bool breakWord, int delayWord);
        SpeedReaderSegment getSpeedReaderSegment(QString value);
        bool stringContainsStopWord(QString value);
        QString prepareTextForDissecting(QString text);
};

#endif // SETTINGS_H
