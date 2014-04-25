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

#include "readwidget.h"
#include <QBoxLayout>

static const char* START = QT_TRANSLATE_NOOP("ReadWidget", "Start");
static const char* STOP = QT_TRANSLATE_NOOP("ReadWidget", "Stop");
static const char* RESTART = QT_TRANSLATE_NOOP("ReadWidget", "Restart");
static const char* PSTSR = QT_TRANSLATE_NOOP("ReadWidget", "Press start to start reading");
static const char* FINISHED = QT_TRANSLATE_NOOP("ReadWidget", "Finished");

ReadWidget::ReadWidget() : QWidget() {
    QBoxLayout* boxLayout = new QBoxLayout(QBoxLayout::TopToBottom);

    mProgressBar = new QProgressBar();
    boxLayout->addWidget(mProgressBar);

    mLabel = new QLabel();
    mLabel->setAlignment(Qt::AlignCenter);
    boxLayout->addWidget(mLabel);

    QHBoxLayout* buttonsBoxLayout = new QHBoxLayout();
    buttonsBoxLayout->setAlignment(Qt::AlignLeft);

    mStartStopButton = new QPushButton();
    buttonsBoxLayout->addWidget(mStartStopButton);

    mCancelButton = new QPushButton();
    buttonsBoxLayout->addWidget(mCancelButton);

    mStartStopButtonChar = START;
    mLabelChar = PSTSR;
    this->retranslate();

    boxLayout->addLayout(buttonsBoxLayout, 0);

    this->setLayout(boxLayout);
}

ReadWidget::~ReadWidget() {
    delete mProgressBar;
    delete mLabel;
    delete mStartStopButton;
    delete mCancelButton;
}

QLabel* ReadWidget::getLabel() {
    return mLabel;
}

void ReadWidget::prepareForReading() {
    mProgressBar->setValue(0);

    this->setStartStopButtonTextStart();

    mLabelChar = PSTSR;
    this->setLabelText();
}

QPushButton* ReadWidget::getStartStopButton() {
    return mStartStopButton;
}

QPushButton* ReadWidget::getCancelButton() {
    return mCancelButton;
}

QProgressBar* ReadWidget::getProgressBar() {
    return mProgressBar;
}

void ReadWidget::retranslate() {
    this->setStartStopButtonText();
    this->setLabelText();
    mCancelButton->setText(QPushButton::tr("Cancel"));
}

void ReadWidget::setStartStopButtonTextStart() {
    mStartStopButtonChar = START;
    this->setStartStopButtonText();
}

void ReadWidget::setStartStopButtonTextStop() {
    mStartStopButtonChar = STOP;
    mLabelChar = 0;
    this->setStartStopButtonText();
}

void ReadWidget::setStartStopButtonTextRestart() {
    mStartStopButtonChar = RESTART;
    this->setStartStopButtonText();
}

void ReadWidget::setStartStopButtonText() {
    mStartStopButton->setText(tr(mStartStopButtonChar));
}

void ReadWidget::setLabelTextFinished() {
    mLabelChar = FINISHED;

    this->setLabelText();
}

void ReadWidget::setLabelText() {
    if (mLabelChar != 0) mLabel->setText(tr(mLabelChar));
}
