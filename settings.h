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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include <QString>
#include <QColor>
#include <QUrl>
#include <QNetworkProxy>
#include <QMap>
#include <QTime>

#include "speedreadersegment.h"
#include "initializableqmap.h"
#include "speedreadertextdatasource.h"

struct Word {
    QString word;
    bool stopWord;
    bool breakWord;
    int delayWord;
};

class Settings : public QObject, public SpeedReaderTextDataSource {
        Q_OBJECT

    signals:
        void updatedSettings();

    public:
        static const int NO_HTTP_NETWORK_PROXY = 0;
        static const int USE_SYSTEM_HTTP_NETWORK_PROXY_CONFIGURATION = 1;
        static const int CUSTOM_HTTP_NETWORK_PROXY = 2;

        static const int MIN_FONT_SIZE = 8;
        static const int MAX_FONT_SIZE = 100;

        static const int MIN_NUMBER_OF_WORDS = 1;
        static const int MAX_NUMBER_OF_WORDS = 10;

        static const int MIN_WORDS_PER_MINUTE = 100;
        static const int MAX_WORDS_PER_MINUTE = 3000;

        static const int MIN_WORD_LENGTH = 5;
        static const int MAX_WORD_LENGTH = 20;

        static const InitializableQMap<int, QString> RSS_REFRESH_RATES;

        static Settings* getInstance();

        void synchronize();

        QColor getTextColor();
        void setTextColor(QColor textColor);
        QColor getTextBackgroundColor();
        void setTextBackgroundColor(QColor textBackgroundColor);
        QColor getLinesColor();
        void setLinesColor(QColor lineColor);
        QString getFontFamily();
        void setFontFamily(QString fontFamily);
        bool shouldDisplayLongerWordsLonger();
        void setShouldDisplayLongerWordsLonger(bool shouldDisplayLongerWordsLonger);
        int getWordLength();
        void setWordLength(int wordLength);
        int getFontSize();
        void setFontSize(int fontSize);
        int getNumberOfWords();
        void setNumberOfWords(int numberOfWords);
        int getWordsPerMinute();
        void setWordsPerMinute(int wordsPerMinute);
        bool shouldGroupNumbers();
        void setShouldGroupNumbers(bool shouldGroupNumbers);
        bool shouldStallAtIndentions();
        void setShouldStallAtIndentions(bool shouldStallAtIndentions);
        bool changedHTTPNetworkProxy();
        int getHTTPNetworkProxyType();
        void setHTTPNetworkProxyType(int httpNetworkProxyType);
        QNetworkProxy getHTTPNetworkProxy();
        void setHTTPNetworkProxy(QNetworkProxy httpNetworkProxy);

        QList<Word> getWords();
        void setWords(QList<Word> stopWords);

        QList<QString> getBreakWords();
        QList<QString> getStopWords();
        QMap<QString, int> getDelayWords();

        QList<QUrl> getRSSSites();
        void setRSSSites(QList<QUrl> rssSites);
        bool appendRSSSite(QUrl rssSite);

        bool changedRSSRefreshRate();
        void setRSSRefreshRate(int rssRefreshRate);
        int getRSSRefreshRate();

        bool autoUpdate();
        void setAutoUpdateTomorrow();
        void setAutoUpdateNeverEver();

        void setMainWindowGeometry(const QByteArray& geometry);
        QByteArray getMainWindowGeometry() const;

        void saveRSSWebViewDialogGeometry(const QByteArray& geometry);
        QByteArray getRSSWebViewDialogGeometry() const;
    private:
        Settings();
        Settings(Settings const&);
        Settings& operator=(Settings const&);
        static Settings* mInstance;

        QColor  mTextColor;
        QColor  mTextBackgroundColor;
        QColor  mLinesColor;
        QString mFontFamily;
        int mFontSize;
        bool mDisplayLongerWordsLonger;
        int mWordLength;
        int mNumberOfWords;
        int mWordsPerMinute;
        bool mNumberGrouping;
        bool mStallAtIndentions;
        int mHTTPNetworkProxyType;
        QNetworkProxy mHTTPNetworkProxy;
        bool mChangedHTTPNetworkProxy;

        QList<Word> mWords;
        QList<QString> mStopWords;
        QList<QString> mBreakWords;
        QMap<QString, int> mDelayWords;
        QList<QUrl> mRSSSites;
        int mRSSRefreshRate;
        bool mChangedRSSRefreshRate;

        void appendWord(Word word);

        uint mAutoUpdate;
        void syncAutoUpdate();

        QByteArray mMainWindowGeometry;
        QByteArray mRSSWebViewDialogGeometry;

        int minMaxValue(int min, int max, int value);
};

#endif // SETTINGS_H
