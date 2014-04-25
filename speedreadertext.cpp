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

#include "speedreadertext.h"

#include "settings.h"

SpeedReaderText::SpeedReaderText(QString text) {
    mText = text;
    mIndex = 0;
    mStopWordsRead = 0;
    mReading = false;
    mStopWordsCount = -1;

    this->dissectText();
}

QString SpeedReaderText::getText() {
    return mText;
}

QString SpeedReaderText::getRemainingText() {
    if (mIndex == 0) return this->getText();

    QString result;

    for (int i = mIndex - 1; i < mDissectText.length(); i++) {
        result.append(QString("%1 ").arg(mDissectText.at(i)));
    }

    return result.trimmed();
}

bool SpeedReaderText::isReading() {
    return mReading;
}

int SpeedReaderText::getIndex() {
    return mIndex;
}

int SpeedReaderText::getMaxIndex() {
    return mDissectText.length();
}

int SpeedReaderText::getStopWordsRead() {
    return mStopWordsRead;
}

int SpeedReaderText::getStopWordsCount() {
    if (mStopWordsCount == -1) { // not cached
        QList<QString> stopWords = Settings::getInstance()->getStopWords();
        mStopWordsCount = 0;

        foreach(QString value, mDissectText) {
            foreach(QString stopWord, stopWords) {
                if(value.contains(stopWord)) {
                    mStopWordsCount++;
                    break; // count only 1 stopword
                }
            }
        }

        return mStopWordsCount;
    }

    return mStopWordsCount;
}

int SpeedReaderText::getEstimatedTimeInMS() {
    int readingTime = Settings::getInstance()->getReadingTimePerMinuteInMs();
    int words = this->getMaxIndex() - mIndex;

    return words * readingTime;
}

QString SpeedReaderText::getHTMLColoredText() {
    QString htmlColoredText = "<span style=\"background-color: rgb(128, 255, 128);\">";
    QList<QString> stopWords = Settings::getInstance()->getStopWords();

    for(int i = 0; i < mDissectText.length(); i++) {
        QString word = mDissectText.at(i);
        QString htmlWord = word + " ";

        if(i == mIndex) { // could also occur a stop word
            htmlWord = "</span><span style=\"background-color: rgb(255, 128, 128);\"><a name=\"myAnchorForCurrentWord\">" + htmlWord + "</a>";

            foreach (QString stopWord, stopWords) {
                if (word.contains(stopWord)) {
                    htmlWord = "</span><span style=\"background-color: rgb(255, 255, 128);\"><a name=\"myAnchorForCurrentWord\">" + word + " </a><span style=\"background-color: rgb(255, 128, 128);\">";
                    break;
                }
            }

            htmlColoredText += htmlWord;
            continue;
        }

        bool foundStopWord = false;

        foreach (QString stopWord, stopWords) {
            if (word.contains(stopWord)) {
                foundStopWord = true;
                break;
            }
        }

        if(foundStopWord) {
            htmlWord = "<span style=\"background-color: rgb(255, 255, 128);\">" + htmlWord + "</span>";
        }

        htmlColoredText += htmlWord;
    }

    return htmlColoredText.trimmed() + "</span>";
}

void SpeedReaderText::stopReading() {
    mReading = false;
}

void SpeedReaderText::continueReading() {
    mReading = true;
}

void SpeedReaderText::resetReading() {
    mReading = false;
    mIndex = 0;
    mStopWordsRead = 0;
    mStopWordsCount = -1;
}

void SpeedReaderText::dissectText() {
    mDissectText = Settings::getInstance()->dissectText(mText);
}

void SpeedReaderText::run() {
    Settings* Settings = Settings::getInstance();
    QList<QString> stopWords = Settings->getStopWords();
    int readingTime = Settings->getReadingTimePerMinuteInMs();
    float floatTextLength = (float) mDissectText.length();

    while(true) {
        bool notFinished = mDissectText.size() > mIndex;

        if(mReading && notFinished) {
            QString word = mDissectText[mIndex];
            SpeedReaderStatus status = SpeedReaderText::READING;

            foreach (QString stopWord, stopWords) {
                if (word.contains(stopWord)) {
                    status = SpeedReaderText::STOPPING;
                    mStopWordsRead++;
                    break; // respect only 1 stop word
                }
            }

            mIndex++;
            emit changed(word, mIndex == 0 ? 0 : (int) ((float) mIndex / floatTextLength * (float) 100), status);

            QThread::msleep(readingTime);
        } else if(notFinished) {
            QThread::msleep(150);
        } else {
            emit changed("", 100, SpeedReaderText::FINISHED);
            break;
        }
    }
}
