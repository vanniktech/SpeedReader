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

#include "settings.h"

#include <QFontDatabase>
#include <QRegularExpression>
#include <QSettings>

Settings* Settings::mInstance = 0;

const QString CO_TEXT_COLOR = "textColor";
const QString CO_TEXT_BACKGROUND_COLOR = "textBackgroundColor";
const QString CO_FONT_FAMILY = "fontFamily";
const QString CO_FONT_SIZE = "fontSize";
const QString CO_DISPLAY_LONGER_WORDS_LONGER = "displayLongerWordsLonger";
const QString CO_WORD_LENGTH = "wordLength";
const QString CO_NUMBER_OF_WORDS = "numberOfWords";
const QString CO_WORDS_PER_MINUTE = "wordsPerMinute";
const QString CO_NUMBER_GROUPING = "numberGrouping";
const QString CO_JUMP_BACK_TO_START = "jumpBackToStart";
const QString CO_STALL_AT_INDENTIONS = "stallAtIndentions";
const QString CO_WORDS = "words";
const QString CO_WORD = "word";
const QString CO_RSS_URLS = "rss_urls";
const QString CO_RSS_URL = "rss_url";
const QString CO_VALUE = "value";
const QString CO_STOP_WORD = "stopWord";
const QString CO_BREAK_WORD = "breakWord";
const QString CO_DELAY_WORD = "delayWord";
const QString CO_HTTP_NETWORK_PROXY_TYPE = "httpNetworkProxyType";
const QString CO_HTTP_NETWORK_PROXY_SERVER = "httpNetworkProxyServer";
const QString CO_HTTP_NETWORK_PROXY_PORT = "httpNetworkProxyPort";

const int MIN_FONT_SIZE = 8;
const int MAX_FONT_SIZE = 100;

const int MIN_NUMBER_OF_WORDS = 1;
const int MAX_NUMBER_OF_WORDS = 10;

const int MIN_WORDS_PER_MINUTE = 300;
const int MAX_WORDS_PER_MINUTE = 3000;

const int MIN_WORDS_LENGTH = 5;
const int MAX_WORDS_LENGTH = 20;

Settings* Settings::getInstance() {
    if (!mInstance) {
        mInstance = new Settings();
    }

    return mInstance;
}

Settings::Settings() {
    QSettings settings;
    this->setFontSize(settings.value(CO_FONT_SIZE, 35).toInt());
    this->setFontFamily(settings.value(CO_FONT_FAMILY, QFontDatabase().families()[0]).toString());
    this->setDisplayLongerWordsLonger(settings.value(CO_DISPLAY_LONGER_WORDS_LONGER, true).toBool());
    this->setWordLength(settings.value(CO_WORD_LENGTH, 8).toInt());
    this->setNumberGrouping(settings.value(CO_NUMBER_GROUPING, true).toBool());
    this->setJumpBackToStart(settings.value(CO_JUMP_BACK_TO_START, false).toBool());
    this->setStallAtIndentions(settings.value(CO_STALL_AT_INDENTIONS, true).toBool());
    this->setNumberOfWords(settings.value(CO_NUMBER_OF_WORDS, 1).toInt());
    this->setWordsPerMinute(settings.value(CO_WORDS_PER_MINUTE, 300).toInt());
    this->setTextBackgroundColor(settings.value(CO_TEXT_BACKGROUND_COLOR, QColor(255, 255, 255)).value<QColor>());
    this->setTextColor(settings.value(CO_TEXT_COLOR, QColor(0, 0, 0)).value<QColor>());
    mHTTPNetworkProxy.setType(QNetworkProxy::HttpProxy);
    mHTTPNetworkProxy.setHostName(settings.value(CO_HTTP_NETWORK_PROXY_SERVER, "proxy").value<QString>());
    mHTTPNetworkProxy.setPort(settings.value(CO_HTTP_NETWORK_PROXY_PORT, 8080).value<quint16>());
    this->setHTTPNetworkProxyType(settings.value(CO_HTTP_NETWORK_PROXY_TYPE, Settings::USE_SYSTEM_HTTP_NETWORK_PROXY_CONFIGURATION).value<int>());

    int size = settings.beginReadArray(CO_WORDS);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);

        QString word = settings.value(CO_VALUE).toString();
        bool stopWord = settings.value(CO_STOP_WORD).toBool();
        bool breakWord = settings.value(CO_BREAK_WORD).toBool();
        int delayWord = qMax(0, settings.value(CO_DELAY_WORD, 0).toInt());
        this->appendWord(word, stopWord, breakWord, delayWord);
    }

    settings.endArray();

    size = settings.beginReadArray(CO_RSS_URLS);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        this->appendRSSUrl(settings.value(CO_RSS_URL).value<QUrl>());
    }

    settings.endArray();

    mChangedWords = mChangedRSSUrls = true;
}

void Settings::synchronize() {
    QSettings settings;

    settings.setValue(CO_TEXT_COLOR, mTextColor);
    settings.setValue(CO_TEXT_BACKGROUND_COLOR, mTextBackgroundColor);
    settings.setValue(CO_FONT_FAMILY, mFontFamily);
    settings.setValue(CO_FONT_SIZE, mFontSize);
    settings.setValue(CO_DISPLAY_LONGER_WORDS_LONGER, mDisplayLongerWordsLonger);
    settings.setValue(CO_WORD_LENGTH, mWordLength);
    settings.setValue(CO_NUMBER_OF_WORDS, mNumberOfWords);
    settings.setValue(CO_WORDS_PER_MINUTE, mWordsPerMinute);
    settings.setValue(CO_NUMBER_GROUPING, mNumberGrouping);
    settings.setValue(CO_JUMP_BACK_TO_START, mJumpBackToStart);
    settings.setValue(CO_HTTP_NETWORK_PROXY_TYPE, mHTTPNetworkProxyType);
    settings.setValue(CO_HTTP_NETWORK_PROXY_SERVER, mHTTPNetworkProxy.hostName());
    settings.setValue(CO_HTTP_NETWORK_PROXY_PORT, mHTTPNetworkProxy.port());

    settings.beginWriteArray(CO_WORDS);
    settings.remove("");

    for (int i = 0; i < mWords.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(CO_VALUE, mWords.at(i).word);
        settings.setValue(CO_STOP_WORD, mWords.at(i).stopWord);
        settings.setValue(CO_BREAK_WORD, mWords.at(i).breakWord);
        settings.setValue(CO_DELAY_WORD, mWords.at(i).delayWord);
    }

    settings.endArray();

    settings.beginWriteArray(CO_RSS_URLS);
    settings.remove("");

    for (int i = 0; i < mRSSUrls.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(CO_RSS_URL, mRSSUrls.at(i));
    }

    settings.endArray();
    settings.sync();

    emit updatedSettings();
}

QString Settings::prepareTextForDissecting(QString text) {
    QString value;
    text = text.replace("\t", " ").trimmed();

    for (int i = 0; i < text.length(); i++) {
        QChar c = text.at(i);
        bool notAtEnd = i + 1 < text.length();

        if ((c != ' ' && c != '\n') || (c == ' ' && notAtEnd && text.at(i + 1) != ' ') || (c == '\n' && notAtEnd && text.at(i + 1) != '\n')) value.append(c);
    }

    return value;
}

QList<SpeedReaderSegment> Settings::dissectText(QString text) {
    const char seperator = ' ';
    int counter = 0;
    text = mStallAtIndentions ? this->prepareTextForDissecting(text) : text.simplified().trimmed();

    QList<SpeedReaderSegment> dissectedText = QList<SpeedReaderSegment>();
    QString tmpText = QString();

    for(int i = 0; i < text.length(); i++) {
        bool indention = mStallAtIndentions && text[i] == '\n';

        if ((text[i] == seperator && ++counter == mNumberOfWords) || indention) {
            dissectedText.append(this->getSpeedReaderSegment(tmpText));
            if (indention) dissectedText.append(this->getSpeedReaderSegment(""));
            tmpText.clear();
            counter = 0;
            continue;
        }

        tmpText.append(text[i]);

        foreach (QString breakWord, mBreakWords) {
            if (tmpText.contains(breakWord)) {
                dissectedText.append(this->getSpeedReaderSegment(tmpText));
                tmpText.clear();
                counter = 0;
                break;
            }
        }
    }

    if (tmpText.trimmed().length() != 0) dissectedText.append(this->getSpeedReaderSegment(tmpText));

    if (mNumberGrouping) {
        QRegularExpression regularExPressionNumber("[0-9]+");
        bool isLastNumber = false;

        for (int i = 0; i < dissectedText.length(); i++) {
            SpeedReaderSegment currentSegment = dissectedText[i];
            bool isNumber = currentSegment.getValue().contains(regularExPressionNumber);

            if (isNumber && isLastNumber) {
                SpeedReaderSegment beforeSegment = dissectedText[i - 1];
                dissectedText.removeAt(i - 1);
                dissectedText.removeAt(i - 1);

                dissectedText.insert(i - 1, this->getSpeedReaderSegment(beforeSegment.getValue() + seperator + currentSegment.getValue()));
                i--;
            }

            isLastNumber = isNumber;
        }
    }

    return dissectedText;
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
    if (fontSize < MIN_FONT_SIZE) {
        fontSize = MIN_FONT_SIZE;
    } else if (fontSize > MAX_FONT_SIZE) {
        fontSize = MAX_FONT_SIZE;
    }

    mFontSize = fontSize;
}

bool Settings::displayLongerWordsLonger() {
    return mDisplayLongerWordsLonger;
}

void Settings::setDisplayLongerWordsLonger(bool displayLongerWordsLonger) {
    mChangedDisplayLongerWordsLonger = mDisplayLongerWordsLonger != displayLongerWordsLonger;
    mDisplayLongerWordsLonger = displayLongerWordsLonger;
}

int Settings::getWordLength() {
    return mWordLength;
}

void Settings::setWordLength(int wordLength) {
    if (wordLength < MIN_WORDS_LENGTH) {
        wordLength = MIN_WORDS_LENGTH;
    } else if (wordLength > MAX_WORDS_LENGTH) {
        wordLength = MAX_WORDS_LENGTH;
    }

    mChangedDisplayLongerWordsLonger = mWordLength != wordLength;
    mWordLength = wordLength;
}

bool Settings::changedDisplayLongerWordsLonger() {
    return mChangedDisplayLongerWordsLonger;
}

int Settings::getNumberOfWords() {
    return mNumberOfWords;
}

void Settings::setNumberOfWords(int numberOfWords) {
    if (numberOfWords < MIN_NUMBER_OF_WORDS) {
        numberOfWords = MIN_NUMBER_OF_WORDS;
    } else if (numberOfWords > MAX_NUMBER_OF_WORDS) {
        numberOfWords = MAX_NUMBER_OF_WORDS;
    }

    mChangedNumberOfWords = mNumberOfWords != numberOfWords;
    mNumberOfWords = numberOfWords;
}

bool Settings::changedNumberOfWords() {
    return mChangedNumberOfWords;
}

int Settings::getWordsPerMinute() {
    return mWordsPerMinute;
}

void Settings::setWordsPerMinute(int wordsPerMinute) {
    if (wordsPerMinute < MIN_WORDS_PER_MINUTE) {
        wordsPerMinute = MIN_WORDS_PER_MINUTE;
    } else if (wordsPerMinute > MAX_WORDS_PER_MINUTE) {
        wordsPerMinute = MAX_WORDS_PER_MINUTE;
    }

    mChangedWordsPerMinute = mWordsPerMinute != wordsPerMinute;
    mWordsPerMinute = wordsPerMinute;
}

bool Settings::changedWordsPerMinute() {
    return mChangedWordsPerMinute;
}

bool Settings::numberGrouping() {
    return mNumberGrouping;
}

void Settings::setNumberGrouping(bool numberGrouping) {
    mChangedNumberGrouping = mNumberGrouping != numberGrouping;
    mNumberGrouping = numberGrouping;
}

bool Settings::changedNumberGrouping() {
    return mChangedNumberGrouping;
}

bool Settings::jumpBackToStart() {
    return mJumpBackToStart;
}

void Settings::setJumpBackToStart(bool jumpBackToStart) {
    mJumpBackToStart = jumpBackToStart;
}

bool Settings::changedStallAtIndentions() {
    return mChangedStallAtIndentions;
}

bool Settings::stallAtIndentions() {
    return mStallAtIndentions;
}

void Settings::setStallAtIndentions(bool stallAtIndentions) {
    mChangedStallAtIndentions = mStallAtIndentions != stallAtIndentions;
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

bool Settings::changedWords() {
    return mChangedWords;
}

void Settings::setWords(QList<Word> words) {
    mChangedWords = mWords.size() != words.size();

    if (!mChangedWords) {
        bool changed = false;
        for (int i = 0; i < words.size(); i++) {
            Word w1 = words.at(i);
            Word w2 = mWords.at(i);

            if (!w1.equals(w2)) {
                changed = true;
                break;
            }
        }

        mChangedWords = changed;
    }

    if (!mChangedWords) return;

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


QList<QUrl> Settings::getRSSUrls() {
    return mRSSUrls;
}

void Settings::setRSSUrls(QList<QUrl> rssUrls) {
    mChangedRSSUrls = mRSSUrls.size() != rssUrls.size();

    if (!mChangedRSSUrls) {
        bool changed = false;
        for (int i = 0; i < rssUrls.size(); i++) {
            if (rssUrls.at(i) != mRSSUrls.at(i)) {
                changed = true;
                break;
            }
        }

        mChangedRSSUrls = changed;
    }

    if (!mChangedRSSUrls) return;

    mRSSUrls = rssUrls;
}

bool Settings::changedRSSUrls() {
    return mChangedRSSUrls;
}

int Settings::getReadingTimePerMinuteInMs() {
    return (60 * 1000) * mNumberOfWords / mWordsPerMinute;
}

void Settings::appendWord(QString value, bool stopWord, bool breakWord, int delayWord) {
    if (value.isEmpty()) return;

    foreach (Word word, mWords) if (word.word == value) return;

    mWords.append(Word(value, stopWord, breakWord, delayWord));

    if (stopWord) mStopWords.append(value);
    if (breakWord) mBreakWords.append(value);
    if (delayWord > 0) mDelayWords.insert(value, delayWord);
}

bool Settings::appendRSSUrl(QUrl rssUrl) {
    if (rssUrl.isValid() && !mRSSUrls.contains(rssUrl)) {
        mRSSUrls.append(rssUrl);

        QSettings settings;
        int index = settings.beginReadArray(CO_RSS_URLS);
        settings.endArray();
        settings.beginWriteArray(CO_RSS_URLS);
        settings.setArrayIndex(index + 1);
        settings.setValue(CO_RSS_URL, rssUrl);
        settings.endArray();

        return true;
    }

    return false;
}

SpeedReaderSegment Settings::getSpeedReaderSegment(QString value) {
    int readingTime = this->getReadingTimePerMinuteInMs();
    int wordLength = mWordLength * mNumberOfWords;
    int valueLength = value.length() - mNumberOfWords + 1; // ignore whitespaces

    if (mDisplayLongerWordsLonger && wordLength < valueLength) readingTime = (int) ((float) readingTime * ((float) valueLength / (float) wordLength));

    QMap<QString, int>::iterator it;
    for (it = mDelayWords.begin(); it != mDelayWords.end(); ++it) readingTime += it.value() * value.count(it.key());

    return SpeedReaderSegment(value, this->stringContainsStopWord(value), readingTime);
}

bool Settings::stringContainsStopWord(QString value) {
    foreach (QString stopWord, mStopWords) if (value.contains(stopWord)) return true;

    return false;
}
