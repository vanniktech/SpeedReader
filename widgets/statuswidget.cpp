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

#include "statuswidget.h"
#include "lib/vntformatter.h"
#include <QVBoxLayout>

StatusWidget::StatusWidget() : QWidget() {
    QVBoxLayout* vBoxLayout = new QVBoxLayout();

    mSegmentsStatusLabel = new QLabel();
    vBoxLayout->addWidget(mSegmentsStatusLabel);

    mStopWordsStatusLabel = new QLabel();
    vBoxLayout->addWidget(mStopWordsStatusLabel);

    mEstimatedReadingTime = new QLabel();
    vBoxLayout->addWidget(mEstimatedReadingTime);

    mTextEdit = new QTextEdit();
    mTextEdit->setReadOnly(true);
    vBoxLayout->addWidget(mTextEdit);

    this->setLayout(vBoxLayout);
}

StatusWidget::~StatusWidget() {
    delete mSegmentsStatusLabel;
    delete mStopWordsStatusLabel;
    delete mEstimatedReadingTime;
    delete mTextEdit;
}

void StatusWidget::update(SpeedReaderText* speedReaderText) {
    mTextEdit->setHtml(speedReaderText->getHTMLColoredText());
    mTextEdit->scrollToAnchor("myAnchorForCurrentWord");

    mSegmentsStatusValue = QString("%1 / %2").arg(QString::number(speedReaderText->getIndex()), QString::number(speedReaderText->getMaxIndex()));
    mStopWordsStatusValue = QString("%1 / %2").arg(QString::number(speedReaderText->getStopWordsRead()), QString::number(speedReaderText->getStopWordsCount()));
    mFormattedTime = VNTFormatter::formatMilliseconds(speedReaderText->getEstimatedTimeInMS());

    this->retranslate();
}

void StatusWidget::retranslate() {
    if (!mSegmentsStatusValue.isNull()) mSegmentsStatusLabel->setText(QString("%1 %2").arg(mSegmentsStatusValue, tr("segments read")));
    if (!mStopWordsStatusValue.isNull()) mStopWordsStatusLabel->setText(QString("%1 %2").arg(mStopWordsStatusValue, tr("stopwords read")));
    if (!mFormattedTime.isNull()) mEstimatedReadingTime->setText(QString(tr("Estimated reamining reading time: %1")).arg(mFormattedTime));
}
