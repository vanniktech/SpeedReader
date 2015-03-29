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

#include "speedreadertext.h"

#include <QRegExp>

const QString SpeedReaderText::HTML_ANCHOR_CURRENT_WORD = "myAnchorForCurrentWord";

const TextParts TextParts::EMPTY_TEXT_PARTS = TextParts("");

SpeedReaderText::SpeedReaderText(SpeedReaderTextDataSource* speedReaderTextDataSource, QObject *parent) : Thread(parent) {
    mSpeedReaderTextDataSource = speedReaderTextDataSource;
    this->setText("");
}

void SpeedReaderText::setText(const QString& text) {
    mText = text;
    mIndex = 0;
    mStopWordsRead = 0;
    mSpeedReading = false;
    mStopWordsCount = -1;

    this->dissectText();
}

QString SpeedReaderText::getText() const {
    return mText;
}

int SpeedReaderText::getIndex() const {
    return mIndex;
}

int SpeedReaderText::getMaxIndex() const {
    return mDissectText.length();
}

int SpeedReaderText::getStopWordsRead() const {
    return mStopWordsRead;
}

int SpeedReaderText::getStopWordsCount() {
    if (mStopWordsCount == -1) { // not cached
        mStopWordsCount = 0;

        for (const SpeedReaderSegment segment : mDissectText) {
            if (segment.isStopWordContained()) {
                mStopWordsCount++;
            }
        }
    }

    return mStopWordsCount;
}

int SpeedReaderText::getEstimatedTimeInMS() const {
    int estimatedTimeInMS = 0;

    for (int i = mIndex; i < mDissectText.size(); i++) {
        estimatedTimeInMS += mDissectText.at(i).getDisplayTime();
    }

    return estimatedTimeInMS;
}

QString SpeedReaderText::getHTMLColoredText() const {
    QString htmlColoredText = "<span style=\"background-color: rgb(128, 255, 128);\">";

    for (int i = 0; i < mDissectText.length(); i++) {
        const SpeedReaderSegment segment = mDissectText.at(i);
        QString value = segment.getValue();
        value = value == "" ? "<br>" : value.toHtmlEscaped(); // empty string = \n
        QString htmlWord = value + " ";

        if (i == mIndex) {
            htmlColoredText += segment.isStopWordContained() ? "</span><span style=\"background-color: rgb(255, 255, 128);\"><a name=\"" + SpeedReaderText::HTML_ANCHOR_CURRENT_WORD + "\">" + value + " </a><span style=\"background-color: rgb(255, 128, 128);\">" : "</span><span style=\"background-color: rgb(255, 128, 128);\"><a name=\"myAnchorForCurrentWord\">" + htmlWord + "</a>";
            continue;
        }

        if (segment.isStopWordContained()) {
            htmlWord = "<span style=\"background-color: rgb(255, 255, 128);\">" + htmlWord + "</span>";
        }

        htmlColoredText += htmlWord;
    }

    return htmlColoredText.trimmed() + "</span>";
}

bool SpeedReaderText::stopSpeedReading() {
    if (this->getMaxIndex() - mIndex != 1) {
        mSpeedReading = false;
    }

    return !mSpeedReading;
}

void SpeedReaderText::continueSpeedReading() {
    mSpeedReading = true;

    if (!this->isRunning()) {
        this->start();
    }
}

void SpeedReaderText::run() {
    const float floatTextLength = (float) mDissectText.length();

    while (!this->isInterruptionRequested()) {
        const bool hasNotFinished = mDissectText.size() > mIndex;

        if (mSpeedReading && hasNotFinished) {
            const SpeedReaderSegment segment = mDissectText[mIndex];
            const SpeedReaderStatus status = segment.isStopWordContained() ? SpeedReaderText::STOPPING : SpeedReaderText::SPEEDREADING;

            if (status == SpeedReaderStatus::STOPPING) {
                mStopWordsRead++;
            }

            mIndex++;
            emit changed(segment.getValue(), mIndex == 0 ? 0 : (int) ((float) mIndex / floatTextLength * (float) 100), status);

            QThread::msleep(segment.getDisplayTime());
        } else if (hasNotFinished) {
            QThread::msleep(150);
        } else {
            emit changed("", 100, SpeedReaderText::FINISHED);
            break;
        }
    }
}

void SpeedReaderText::dissectText() {
    const char separator = ' ';
    const bool shouldStallAtIndentions = mSpeedReaderTextDataSource->shouldStallAtIndentions();
    const bool shouldGroupNumbers = mSpeedReaderTextDataSource->shouldGroupNumbers();
    const int numberOfWords = mSpeedReaderTextDataSource->getNumberOfWords();
    QList<QString> breakWords = mSpeedReaderTextDataSource->getBreakWords();
    QMap<QString, int> delayWords = mSpeedReaderTextDataSource->getDelayWords();
    QList<QString> stopWords = mSpeedReaderTextDataSource->getStopWords();

    int counter = 0;
    QString text = shouldStallAtIndentions ? this->prepareTextForDissecting(mText) : mText.simplified();

    QList<SpeedReaderSegment> dissectedText;
    TextParts textParts;
    QStringList parts = text.split(separator);

    for (int i = 0; i < parts.size(); i++) {
        const QString part = parts.at(i);

        if (part.isEmpty()) {
            continue;
        }

        textParts.add(part);

        const bool isIndention = shouldStallAtIndentions && part.contains("\n");

        if (isIndention) {
            QStringList indentionQStrings = part.split("\n");
            int lastIndex = indentionQStrings.size() - 1;

            for (int k = 0; k < lastIndex; k++) {
                QString indentionQString = indentionQStrings.at(k);
                dissectedText.append(this->getSpeedReaderSegment(delayWords, stopWords, TextParts(indentionQString)));
                dissectedText.append(this->getSpeedReaderSegment(delayWords, stopWords, TextParts::EMPTY_TEXT_PARTS));
            }

            parts.insert(i + 1, indentionQStrings[lastIndex]);
            counter = 0;
            textParts.clear();
            continue;
        }

        if (++counter == numberOfWords || breakWords.contains(part)) {
            dissectedText.append(this->getSpeedReaderSegment(delayWords, stopWords, textParts));
            counter = 0;
            textParts.clear();
        }
    }

    if (textParts.size() > 0) {
        dissectedText.append(this->getSpeedReaderSegment(delayWords, stopWords, textParts));
    }

    if (shouldGroupNumbers) {
        const QRegExp regularExpressionNumber("([\\d]+ ?)+");
        bool isLastNumber = false;

        for (int i = 0; i < dissectedText.size(); i++) {
            const SpeedReaderSegment currentSegment = dissectedText.at(i);
            const bool isNumber = regularExpressionNumber.exactMatch(currentSegment.getValue());

            if (isNumber && isLastNumber) {
                const SpeedReaderSegment beforeSegment = dissectedText.at(i - 1);
                dissectedText.removeAt(i - 1);
                dissectedText.removeAt(i - 1);

                const SpeedReaderSegment speedReaderSegment = this->getSpeedReaderSegment(delayWords, stopWords, TextParts(beforeSegment.getValue() + separator + currentSegment.getValue()));
                const int dissectedTextSize = dissectedText.size();

                if (dissectedTextSize == 0 || dissectedTextSize == i - 1) {
                    dissectedText.append(speedReaderSegment);
                } else {
                    dissectedText.insert(i - 1, speedReaderSegment);
                }

                i--;
            }

            isLastNumber = isNumber;
        }
    }

    mDissectText = dissectedText;
}

QString SpeedReaderText::prepareTextForDissecting(QString input) const {
    QString value;

    const QString text = input.replace("\t", " ").trimmed();

    for (int i = 0; i < text.length(); i++) {
        const QChar c = text.at(i);

        const bool isNotAtEnd = i + 1 < text.length();
        const bool isCWhitespace = c == ' ';
        const bool isCNewLine = c == '\n';

        if ((!isCWhitespace && !isCNewLine) || (isCWhitespace && isNotAtEnd && text.at(i + 1) != ' ') || (isCNewLine && isNotAtEnd && text.at(i + 1) != '\n')) {
            value.append(c);
        }
    }

    return value;
}

SpeedReaderSegment SpeedReaderText::getSpeedReaderSegment(QMap<QString, int> delayWords, const QList<QString>& stopWords, const TextParts& textParts) const {
    const QString string = textParts.toString();
    const int stringLength = string.length();
    int speedreadingTime = this->getSpeedReadingTimePerMinuteInMs();

    if (stringLength > 0) {
        const int wordLength = mSpeedReaderTextDataSource->getWordLength() * mSpeedReaderTextDataSource->getNumberOfWords();
        const int valueLength = stringLength - mSpeedReaderTextDataSource->getNumberOfWords() + 1; // ignore whitespaces

        if (mSpeedReaderTextDataSource->shouldDisplayLongerWordsLonger() && wordLength < valueLength) {
            speedreadingTime = (int) ((float) speedreadingTime * ((float) valueLength / (float) wordLength));
        }

        QMap<QString, int>::iterator it;

        for (it = delayWords.begin(); it != delayWords.end(); ++it) {
            const QString delayWordKey = it.key();
            const int numberOfOccurrences = textParts.numberOfOccurrences(delayWordKey);

            if (numberOfOccurrences > 0) {
                const int delayWordTime = it.value();
                speedreadingTime += numberOfOccurrences * delayWordTime;
            }
        }
    }

    return SpeedReaderSegment(string, textParts.contains(stopWords), speedreadingTime);
}

int SpeedReaderText::getSpeedReadingTimePerMinuteInMs() const {
    return (60 * 1000) * mSpeedReaderTextDataSource->getNumberOfWords() / mSpeedReaderTextDataSource->getWordsPerMinute();
}
