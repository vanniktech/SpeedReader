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
const QString CO_NUMBER_OF_WORDS = "numberOfWords";
const QString CO_WORDS_PER_MINUTE = "wordsPerMinute";
const QString CO_NUMBER_GROUPING = "numberGrouping";
const QString CO_JUMP_BACK_TO_START = "jumpBackToStart";
const QString CO_WORDS = "words";
const QString CO_WORD = "word";
const QString CO_RSS_URLS = "rss_urls";
const QString CO_RSS_URL = "rss_url";
const QString CO_VALUE = "value";
const QString CO_STOP_WORD = "stopWord";
const QString CO_BREAK_WORD = "breakWord";

const int MIN_FONT_SIZE = 8;
const int MAX_FONT_SIZE = 100;

const int MIN_NUMBER_OF_WORDS = 1;
const int MAX_NUMBER_OF_WORDS = 10;

const int MIN_WORDS_PER_MINUTE = 300;
const int MAX_WORDS_PER_MINUTE = 3000;

Settings* Settings::getInstance() {
    if (!mInstance) {
        mInstance = new Settings();
    }

    return mInstance;
}

Settings::Settings() {
    QSettings settings;
    mFontSize = settings.value(CO_FONT_SIZE, 35).toInt();
    mFontFamily = settings.value(CO_FONT_FAMILY, QFontDatabase().families()[0]).toString();
    mNumberGrouping = settings.value(CO_NUMBER_GROUPING, true).toBool();
    mJumpBackToStart = settings.value(CO_JUMP_BACK_TO_START, false).toBool();
    mNumberOfWords = settings.value(CO_NUMBER_OF_WORDS, 1).toInt();
    mWordsPerMinute = settings.value(CO_WORDS_PER_MINUTE, 300).toInt();
    mTextBackgroundColor = settings.value(CO_TEXT_BACKGROUND_COLOR, QColor(255, 255, 255)).value<QColor>();
    mTextColor = settings.value(CO_TEXT_COLOR, QColor(0, 0, 0)).value<QColor>();

    int size = settings.beginReadArray(CO_WORDS);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);

        QString word = settings.value(CO_VALUE).toString();
        bool stopWord = settings.value(CO_STOP_WORD).toBool();
        bool breakWord = settings.value(CO_BREAK_WORD).toBool();
        this->appendStopWord(word, stopWord, breakWord);
    }

    settings.endArray();

    size = settings.beginReadArray(CO_RSS_URLS);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);

        this->appendRSSUrl(settings.value(CO_RSS_URL).value<QUrl>());
    }

    settings.endArray();
}

void Settings::synchronize() {
    QSettings settings;

    settings.setValue(CO_TEXT_COLOR, mTextColor);
    settings.setValue(CO_TEXT_BACKGROUND_COLOR, mTextBackgroundColor);
    settings.setValue(CO_FONT_FAMILY, mFontFamily);
    settings.setValue(CO_FONT_SIZE, mFontSize);
    settings.setValue(CO_NUMBER_OF_WORDS, mNumberOfWords);
    settings.setValue(CO_WORDS_PER_MINUTE, mWordsPerMinute);
    settings.setValue(CO_NUMBER_GROUPING, mNumberGrouping);
    settings.setValue(CO_JUMP_BACK_TO_START, mJumpBackToStart);

    settings.beginWriteArray(CO_WORDS);

    for (int i = 0; i < mWords.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(CO_VALUE, mWords.at(i).word);
        settings.setValue(CO_STOP_WORD, mWords.at(i).stopWord);
        settings.setValue(CO_BREAK_WORD, mWords.at(i).breakWord);
    }

    settings.endArray();

    settings.beginWriteArray(CO_RSS_URLS);

    for (int i = 0; i < mRSSUrls.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(CO_RSS_URL, mRSSUrls.at(i));
    }

    settings.endArray();
    settings.sync();

    emit updatedSettings();
}

QList<QString> Settings::dissectText(QString text) {
    const char seperator = ' ';
    int counter = 0;
    text = text.simplified().trimmed();

    QList<QString> dissectedText = QList<QString>();
    QString tmpText = QString();

    for(int i = 0; i < text.length(); i++) {
        if (text[i] == seperator && ++counter == mNumberOfWords) {
            dissectedText.append(tmpText);
            tmpText.clear();
            counter = 0;
            continue;
        }

        tmpText.append(text[i]);

        foreach (QString breakWord, mBreakWords) {
            if (tmpText.contains(breakWord)) {
                dissectedText.append(tmpText);
                tmpText.clear();
                counter = 0;

                if (text[i + 1] == ' ') i++; // ignore next space for right order

                break;
            }
        }
    }

    if (tmpText.length() != 0 && tmpText != " " ) dissectedText.append(tmpText);

    if (mNumberGrouping) {
        QRegularExpression regularExPressionNumber("[0-9]+");
        bool isLastNumber = false;

        for (int i = 0; i < dissectedText.length(); i++) {
            QString current = dissectedText[i];
            bool isNumber = current.contains(regularExPressionNumber);

            if (isNumber && isLastNumber) {
                QString before = dissectedText[i - 1];
                dissectedText.removeAt(i - 1);
                dissectedText.removeAt(i - 1);

                dissectedText.insert(i - 1, before + seperator + current);
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
    if(fontSize < MIN_FONT_SIZE) {
        fontSize = MIN_FONT_SIZE;
    } else if(fontSize > MAX_FONT_SIZE) {
        fontSize = MAX_FONT_SIZE;
    }

    mFontSize = fontSize;
}

int Settings::getNumberOfWords() {
    return mNumberOfWords;
}

void Settings::setNumberOfWords(int numberOfWords) {
    if(numberOfWords < MIN_NUMBER_OF_WORDS) {
        numberOfWords = MIN_NUMBER_OF_WORDS;
    } else if(numberOfWords > MAX_NUMBER_OF_WORDS) {
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
    if(wordsPerMinute < MIN_WORDS_PER_MINUTE) {
        wordsPerMinute = MIN_WORDS_PER_MINUTE;
    } else if(wordsPerMinute > MAX_WORDS_PER_MINUTE) {
        wordsPerMinute = MAX_WORDS_PER_MINUTE;
    }

    mChangedWordsPerMinute = mWordsPerMinute != wordsPerMinute;
    mWordsPerMinute = wordsPerMinute;
}

bool Settings::changedWordsPerMinute() {
    return mChangedWordsPerMinute;
}

bool Settings::isNumberGrouping() {
    return mNumberGrouping;
}

void Settings::setNumberGrouping(bool numberGrouping) {
    mChangedNumberGrouping = mNumberGrouping != numberGrouping;
    mNumberGrouping = numberGrouping;
}

bool Settings::changedNumberGrouping() {
    return mChangedNumberGrouping;
}

bool Settings::isJumpBackToStart() {
    return mJumpBackToStart;
}

void Settings::setJumpBackToStart(bool jumpBackToStart) {
    mJumpBackToStart = jumpBackToStart;
}

QList<Word> Settings::getWords() {
    return mWords;
}

bool Settings::changedWords() {
    return mChangedWords;
}

QList<QString> Settings::getStopWords() {
    return mStopWords;
}

QList<QString> Settings::getBreakWords() {
    return mBreakWords;
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

    for(int i = 0; i < mWords.count(); i++) {
        if (mWords.at(i).stopWord) mStopWords.append(mWords.at(i).word);
        if (mWords.at(i).breakWord) mBreakWords.append(mWords.at(i).word);
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

void Settings::appendStopWord(QString value, bool stopWord, bool breakWord) {
    if (value.isEmpty()) return;

    foreach(Word word, mWords) {
        if (word.word == value) return;
    }

    mWords.append(Word(value, stopWord, breakWord));

    if (stopWord) mStopWords.append(value);
    if (breakWord) mBreakWords.append(value);
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
