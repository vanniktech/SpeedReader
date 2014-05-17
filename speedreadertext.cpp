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
    mEstimatedTimeInMS = -1;

    this->dissectText();
}

QString SpeedReaderText::getText() {
    return mText;
}

QString SpeedReaderText::getRemainingText() {
    if (mIndex == 0) return this->getText();

    QString result;

    for (int i = mIndex - 1; i < mDissectText.length(); i++) {
        result.append(QString("%1 ").arg(mDissectText.at(i).getValue()));
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
        mStopWordsCount = 0;

        foreach (SpeedReaderSegment segment, mDissectText) {
            if (segment.isStopWordContained()) mStopWordsCount++;
        }
    }

    return mStopWordsCount;
}

int SpeedReaderText::getEstimatedTimeInMS() {
    if (mEstimatedTimeInMS == -1) { // not cached
        mEstimatedTimeInMS = 0;

        for (int i = mIndex; i < this->getMaxIndex(); i++) {
            mEstimatedTimeInMS += mDissectText[i].getDisplayTime();
        }
    }

    return mEstimatedTimeInMS;
}

QString SpeedReaderText::getHTMLColoredText() {
    QString htmlColoredText = "<span style=\"background-color: rgb(128, 255, 128);\">";

    for (int i = 0; i < mDissectText.length(); i++) {
        SpeedReaderSegment segment = mDissectText.at(i);
        QString value = segment.getValue();
        value = value == "" ? "<br>" : value.toHtmlEscaped(); // empty string = \n
        QString htmlWord = value + " ";

        if (i == mIndex) {
            htmlColoredText += segment.isStopWordContained() ? "</span><span style=\"background-color: rgb(255, 255, 128);\"><a name=\"myAnchorForCurrentWord\">" + value + " </a><span style=\"background-color: rgb(255, 128, 128);\">" : "</span><span style=\"background-color: rgb(255, 128, 128);\"><a name=\"myAnchorForCurrentWord\">" + htmlWord + "</a>";
            continue;
        }

        if (segment.isStopWordContained()) {
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
    float floatTextLength = (float) mDissectText.length();

    while(true) {
        bool notFinished = mDissectText.size() > mIndex;

        if(mReading && notFinished) {
            SpeedReaderSegment segment = mDissectText[mIndex];
            SpeedReaderStatus status = SpeedReaderText::READING;

            if (segment.isStopWordContained()) {
                status = SpeedReaderText::STOPPING;
                mStopWordsRead++;
            }

            mIndex++;
            emit changed(segment.getValue(), mIndex == 0 ? 0 : (int) ((float) mIndex / floatTextLength * (float) 100), status);

            int segmentDisplayTime = segment.getDisplayTime();
            mEstimatedTimeInMS -= segmentDisplayTime;
            QThread::msleep(segmentDisplayTime);
        } else if(notFinished) {
            QThread::msleep(150);
        } else {
            emit changed("", 100, SpeedReaderText::FINISHED);
            break;
        }
    }
}
