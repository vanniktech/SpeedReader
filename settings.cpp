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

#include "settings.h"

#include <QFontDatabase>
#include <QSettings>

const InitializableQMap<int, QString> Settings::RSS_REFRESH_RATES = InitializableQMap<int, QString>()
<< QPair<int, QString>(0, tr("never"))
<< QPair<int, QString>(300000, tr("5 minutes"))
<< QPair<int, QString>(600000, tr("10 minutes"))
<< QPair<int, QString>(900000, tr("15 minutes"))
<< QPair<int, QString>(1800000, tr("30 minutes"))
<< QPair<int, QString>(3600000, tr("1 hour"))
<< QPair<int, QString>(7200000, tr("2 hours"))
<< QPair<int, QString>(14400000, tr("4 hours"))
<< QPair<int, QString>(28800000, tr("8 hours"))
<< QPair<int, QString>(43200000, tr("12 hours"))
<< QPair<int, QString>(86400000, tr("24 hours"));

Settings* Settings::mInstance = 0;

const QString SETTINGS_TEXT_COLOR = "textColor";
const QString SETTINGS_TEXT_BACKGROUND_COLOR = "textBackgroundColor";
const QString SETTINGS_LINES_COLOR = "linesColor";
const QString SETTINGS_FONT_FAMILY = "fontFamily";
const QString SETTINGS_FONT_SIZE = "fontSize";
const QString SETTINGS_DISPLAY_LONGER_WORDS_LONGER = "displayLongerWordsLonger";
const QString SETTINGS_WORD_LENGTH = "wordLength";
const QString SETTINGS_NUMBER_OF_WORDS = "numberOfWords";
const QString SETTINGS_WORDS_PER_MINUTE = "wordsPerMinute";
const QString SETTINGS_NUMBER_GROUPING = "numberGrouping";
const QString SETTINGS_STALL_AT_INDENTIONS = "stallAtIndentions";
const QString SETTINGS_WORDS = "words";
const QString SETTINGS_WORD = "word";
const QString SETTINGS_RSS_SITES = "rss_urls";
const QString SETTINGS_RSS_SITE = "rss_url";
const QString SETTINGS_VALUE = "value";
const QString SETTINGS_STOP_WORD = "stopWord";
const QString SETTINGS_BREAK_WORD = "breakWord";
const QString SETTINGS_DELAY_WORD = "delayWord";
const QString SETTINGS_HTTP_NETWORK_PROXY_TYPE = "httpNetworkProxyType";
const QString SETTINGS_HTTP_NETWORK_PROXY_SERVER = "httpNetworkProxyServer";
const QString SETTINGS_HTTP_NETWORK_PROXY_PORT = "httpNetworkProxyPort";
const QString SETTINGS_OPEN_EXTERNALLY_RSS_FEED_ITEM = "openExternallyRSSFeedItem";
const QString SETTINGS_RSS_REFRESH_RATE = "rssRefreshRate";
const QString SETTINGS_AUTO_UPDATE = "autoUpdate" + QString(APPLICATION_VERSION);
const QString SETTINGS_MAIN_WINDOW_GEOMETRY = "mainWindowGeometry";
const QString SETTINGS_RSS_WEB_VIEW_DIALOG_GEOMETRY = "settingsRSSWebViewDialogGeometry";

const int FONT_SIZE_DEFAULT = 35;
const bool DISPLAY_LONGER_WORDS_LONGER_DEFAULT = true;
const int WORD_LENGTH_DEFAULT = 8;
const int NUMBER_OF_WORDS = 1;
const bool NUMER_GROUPING_DEFAULT = true;
const bool STALL_AT_INDENTIONS_DEFAULT = true;
const int WORDS_PER_MINUTE = 300;
const QColor TEXT_BACKGROUND_COLOR_DEFAULT = QColor(255, 255, 255);
const QColor TEXT_COLOR_DEFAULT = QColor(0, 0, 0);
const QColor LINES_COLOR_DEFAULT = QColor(28, 153, 255);
const QString HTTP_NETWORK_PROXY_SERVER_DEFAULT = "proxy";
const int HTTP_NETWORK_PROXY_PORT_DEFAULT = 8080;
const int HTTP_NETWORK_PROXY_TYPE_DEFAULT = Settings::USE_SYSTEM_HTTP_NETWORK_PROXY_CONFIGURATION;
const int RSS_REFRESH_RATE_DEFAULT = Settings::RSS_REFRESH_RATES.lastKey();

Settings* Settings::getInstance() {
    if (!mInstance) {
        mInstance = new Settings();
    }

    return mInstance;
}

Settings::Settings() {
    QSettings settings;
    this->setFontSize(settings.value(SETTINGS_FONT_SIZE, FONT_SIZE_DEFAULT).toInt());
    this->setFontFamily(settings.value(SETTINGS_FONT_FAMILY, QFontDatabase().families()[0]).toString());
    this->setShouldDisplayLongerWordsLonger(settings.value(SETTINGS_DISPLAY_LONGER_WORDS_LONGER, DISPLAY_LONGER_WORDS_LONGER_DEFAULT).toBool());
    this->setWordLength(settings.value(SETTINGS_WORD_LENGTH, WORD_LENGTH_DEFAULT).toInt());
    this->setShouldGroupNumbers(settings.value(SETTINGS_NUMBER_GROUPING, NUMER_GROUPING_DEFAULT).toBool());
    this->setShouldStallAtIndentions(settings.value(SETTINGS_STALL_AT_INDENTIONS, STALL_AT_INDENTIONS_DEFAULT).toBool());
    this->setNumberOfWords(settings.value(SETTINGS_NUMBER_OF_WORDS, NUMBER_OF_WORDS).toInt());
    this->setWordsPerMinute(settings.value(SETTINGS_WORDS_PER_MINUTE, WORDS_PER_MINUTE).toInt());
    this->setTextBackgroundColor(settings.value(SETTINGS_TEXT_BACKGROUND_COLOR, TEXT_BACKGROUND_COLOR_DEFAULT).value<QColor>());
    this->setTextColor(settings.value(SETTINGS_TEXT_COLOR, TEXT_COLOR_DEFAULT).value<QColor>());
    this->setLinesColor(settings.value(SETTINGS_LINES_COLOR, LINES_COLOR_DEFAULT).value<QColor>());
    mHTTPNetworkProxy.setType(QNetworkProxy::HttpProxy);
    mHTTPNetworkProxy.setHostName(settings.value(SETTINGS_HTTP_NETWORK_PROXY_SERVER, HTTP_NETWORK_PROXY_SERVER_DEFAULT).value<QString>());
    mHTTPNetworkProxy.setPort(settings.value(SETTINGS_HTTP_NETWORK_PROXY_PORT, HTTP_NETWORK_PROXY_PORT_DEFAULT).value<quint16>());
    this->setHTTPNetworkProxyType(settings.value(SETTINGS_HTTP_NETWORK_PROXY_TYPE, HTTP_NETWORK_PROXY_TYPE_DEFAULT).value<int>());
    this->setRSSRefreshRate(settings.value(SETTINGS_RSS_REFRESH_RATE, RSS_REFRESH_RATE_DEFAULT).value<int>());
    mAutoUpdate = settings.value(SETTINGS_AUTO_UPDATE, 1).value<int>();
    mMainWindowGeometry = settings.value(SETTINGS_MAIN_WINDOW_GEOMETRY, QByteArray()).value<QByteArray>();
    mRSSWebViewDialogGeometry = settings.value(SETTINGS_RSS_WEB_VIEW_DIALOG_GEOMETRY, QByteArray()).value<QByteArray>();

    int size = settings.beginReadArray(SETTINGS_WORDS);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);

        Word word;
        word.word = settings.value(SETTINGS_VALUE).toString();
        word.stopWord = settings.value(SETTINGS_STOP_WORD).toBool();
        word.breakWord = settings.value(SETTINGS_BREAK_WORD).toBool();
        word.delayWord = qMax(0, settings.value(SETTINGS_DELAY_WORD, 0).toInt());
        this->appendWord(word);
    }

    settings.endArray();

    size = settings.beginReadArray(SETTINGS_RSS_SITES);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);

        QUrl url = settings.value(SETTINGS_RSS_SITE).value<QUrl>();
        if (url.isValid() && !mRSSSites.contains(url)) mRSSSites.append(url);
    }

    settings.endArray();

    mChangedRSSRefreshRate = mChangedHTTPNetworkProxy = true;
}

void Settings::synchronize() {
    QSettings settings;

    settings.setValue(SETTINGS_TEXT_COLOR, mTextColor);
    settings.setValue(SETTINGS_TEXT_BACKGROUND_COLOR, mTextBackgroundColor);
    settings.setValue(SETTINGS_LINES_COLOR, mLinesColor);
    settings.setValue(SETTINGS_FONT_FAMILY, mFontFamily);
    settings.setValue(SETTINGS_FONT_SIZE, mFontSize);
    settings.setValue(SETTINGS_DISPLAY_LONGER_WORDS_LONGER, mDisplayLongerWordsLonger);
    settings.setValue(SETTINGS_WORD_LENGTH, mWordLength);
    settings.setValue(SETTINGS_NUMBER_OF_WORDS, mNumberOfWords);
    settings.setValue(SETTINGS_WORDS_PER_MINUTE, mWordsPerMinute);
    settings.setValue(SETTINGS_NUMBER_GROUPING, mNumberGrouping);
    settings.setValue(SETTINGS_HTTP_NETWORK_PROXY_TYPE, mHTTPNetworkProxyType);
    settings.setValue(SETTINGS_HTTP_NETWORK_PROXY_SERVER, mHTTPNetworkProxy.hostName());
    settings.setValue(SETTINGS_HTTP_NETWORK_PROXY_PORT, mHTTPNetworkProxy.port());
    settings.setValue(SETTINGS_RSS_REFRESH_RATE, mRSSRefreshRate);

    settings.beginWriteArray(SETTINGS_WORDS);
    settings.remove("");

    for (int i = 0; i < mWords.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(SETTINGS_VALUE, mWords.at(i).word);
        settings.setValue(SETTINGS_STOP_WORD, mWords.at(i).stopWord);
        settings.setValue(SETTINGS_BREAK_WORD, mWords.at(i).breakWord);
        settings.setValue(SETTINGS_DELAY_WORD, mWords.at(i).delayWord);
    }

    settings.endArray();

    settings.beginWriteArray(SETTINGS_RSS_SITES);
    settings.remove("");

    for (int i = 0; i < mRSSSites.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(SETTINGS_RSS_SITE, mRSSSites.at(i));
    }

    settings.endArray();
    settings.sync();

    emit updatedSettings();
}

QColor Settings::getTextColor() {
    return mTextColor;
}

void Settings::setTextColor(QColor textColor) {
    mTextColor = textColor;
}

QColor Settings::getTextBackgroundColor() {
    return mTextBackgroundColor;
}

void Settings::setTextBackgroundColor(QColor textBackgroundColor) {
    mTextBackgroundColor = textBackgroundColor;
}

QColor Settings::getLinesColor() {
    return mLinesColor;
}

void Settings::setLinesColor(QColor lineColor) {
    mLinesColor = lineColor;
}

QString Settings::getFontFamily() {
    return mFontFamily;
}

void Settings::setFontFamily(QString fontFamily) {
    QStringList families = QFontDatabase().families();

    mFontFamily = families.contains(fontFamily) ? fontFamily : families[0];
}

int Settings::getFontSize() {
    return mFontSize;
}

void Settings::setFontSize(int fontSize) {
    mFontSize = this->minMaxValue(MIN_FONT_SIZE, MAX_FONT_SIZE, fontSize);
}

bool Settings::shouldDisplayLongerWordsLonger() {
    return mDisplayLongerWordsLonger;
}

void Settings::setShouldDisplayLongerWordsLonger(bool displayLongerWordsLonger) {
    mDisplayLongerWordsLonger = displayLongerWordsLonger;
}

int Settings::getWordLength() {
    return mWordLength;
}

void Settings::setWordLength(int wordLength) {
    mWordLength = this->minMaxValue(MIN_WORD_LENGTH, MAX_WORD_LENGTH, wordLength);
}

int Settings::getNumberOfWords() {
    return mNumberOfWords;
}

void Settings::setNumberOfWords(int numberOfWords) {
    mNumberOfWords = this->minMaxValue(MIN_NUMBER_OF_WORDS, MAX_NUMBER_OF_WORDS, numberOfWords);
}

int Settings::getWordsPerMinute() {
    return mWordsPerMinute;
}

void Settings::setWordsPerMinute(int wordsPerMinute) {
    mWordsPerMinute = this->minMaxValue(MIN_WORDS_PER_MINUTE, MAX_WORDS_PER_MINUTE, wordsPerMinute);
}

bool Settings::shouldGroupNumbers() {
    return mNumberGrouping;
}

void Settings::setShouldGroupNumbers(bool shouldGroupNumbers) {
    mNumberGrouping = shouldGroupNumbers;
}

bool Settings::shouldStallAtIndentions() {
    return mStallAtIndentions;
}

void Settings::setShouldStallAtIndentions(bool stallAtIndentions) {
    mStallAtIndentions = stallAtIndentions;
}

bool Settings::changedHTTPNetworkProxy() {
    return mChangedHTTPNetworkProxy;
}

int Settings::getHTTPNetworkProxyType() {
    return mHTTPNetworkProxyType;
}

void Settings::setHTTPNetworkProxyType(int httpNetworkProxyType) {
    switch (httpNetworkProxyType) {
        case NO_HTTP_NETWORK_PROXY:
        case USE_SYSTEM_HTTP_NETWORK_PROXY_CONFIGURATION:
        case CUSTOM_HTTP_NETWORK_PROXY:
            mChangedHTTPNetworkProxy = mHTTPNetworkProxyType != httpNetworkProxyType;
            mHTTPNetworkProxyType = httpNetworkProxyType;
            break;
    }
}

QNetworkProxy Settings::getHTTPNetworkProxy() {
    return mHTTPNetworkProxy;
}

void Settings::setHTTPNetworkProxy(QNetworkProxy httpNetworkProxy) {
    mChangedHTTPNetworkProxy = mHTTPNetworkProxy.hostName() != httpNetworkProxy.hostName() || mHTTPNetworkProxy.port() != httpNetworkProxy.port();
    mHTTPNetworkProxy = httpNetworkProxy;
}

QList<Word> Settings::getWords() {
    return mWords;
}

QList<QString> Settings::getBreakWords() {
    return mBreakWords;
}

QList<QString> Settings::getStopWords() {
    return mStopWords;
}

QMap<QString, int> Settings::getDelayWords() {
    return mDelayWords;
}

void Settings::setWords(QList<Word> words) {
    mWords = words;

    mStopWords = QList<QString>();
    mBreakWords = QList<QString>();
    mDelayWords = QMap<QString, int>();

    for (int i = 0; i < mWords.count(); i++) {
        Word word = mWords.at(i);

        if (word.stopWord) mStopWords.append(word.word);
        if (word.breakWord) mBreakWords.append(word.word);
        if (word.delayWord > 0) mDelayWords.insert(word.word, word.delayWord);
    }
}


QList<QUrl> Settings::getRSSSites() {
    return mRSSSites;
}

void Settings::setRSSSites(QList<QUrl> rssSites) {
    mRSSSites = rssSites;
}

void Settings::appendWord(Word word) {
    if (word.word.isEmpty()) return;

    foreach (Word mWord, mWords) if (mWord.word == word.word) return;

    mWords.append(word);

    if (word.stopWord) mStopWords.append(word.word);
    if (word.breakWord) mBreakWords.append(word.word);
    if (word.delayWord > 0) mDelayWords.insert(word.word, word.delayWord);
}

bool Settings::appendRSSSite(QUrl rssSite) {
    if (rssSite.isValid() && !mRSSSites.contains(rssSite)) {
        mRSSSites.append(rssSite);

        QSettings settings;
        int index = settings.beginReadArray(SETTINGS_RSS_SITES);
        settings.endArray();
        settings.beginWriteArray(SETTINGS_RSS_SITES);
        settings.setArrayIndex(index + 1);
        settings.setValue(SETTINGS_RSS_SITE, rssSite);
        settings.endArray();

        return true;
    }

    return false;
}


bool Settings::changedRSSRefreshRate() {
    return mChangedRSSRefreshRate;
}

void Settings::setRSSRefreshRate(int rssRefreshRate) {
    if (!RSS_REFRESH_RATES.contains(rssRefreshRate)) rssRefreshRate = RSS_REFRESH_RATE_DEFAULT;

    mChangedRSSRefreshRate = mRSSRefreshRate != rssRefreshRate;
    mRSSRefreshRate = rssRefreshRate;
}

int Settings::getRSSRefreshRate() {
    return mRSSRefreshRate;
}

bool Settings::autoUpdate() {
    return mAutoUpdate != 0 && QDateTime::currentMSecsSinceEpoch() / 1000L >= mAutoUpdate;
}

void Settings::setAutoUpdateTomorrow() {
    mAutoUpdate = QDateTime::currentDateTime().addDays(1).toTime_t();
    this->syncAutoUpdate();
}

void Settings::setAutoUpdateNeverEver() {
    mAutoUpdate = 0;
    this->syncAutoUpdate();
}

void Settings::syncAutoUpdate() {
    QSettings settings;
    settings.setValue(SETTINGS_AUTO_UPDATE, mAutoUpdate);
    settings.sync();
}

void Settings::setMainWindowGeometry(const QByteArray& geometry) {
    mMainWindowGeometry = geometry;

    QSettings settings;
    settings.setValue(SETTINGS_MAIN_WINDOW_GEOMETRY, mMainWindowGeometry);
    settings.sync();
}

QByteArray Settings::getMainWindowGeometry() const {
    return mMainWindowGeometry;
}

void Settings::saveRSSWebViewDialogGeometry(const QByteArray& geometry) {
    mRSSWebViewDialogGeometry = geometry;

    QSettings settings;
    settings.setValue(SETTINGS_RSS_WEB_VIEW_DIALOG_GEOMETRY, mRSSWebViewDialogGeometry);
    settings.sync();
}

QByteArray Settings::getRSSWebViewDialogGeometry() const {
    return mRSSWebViewDialogGeometry;
}

int Settings::minMaxValue(int min, int max, int value) {
    return value < min ? min : (value > max ? max : value);
}
