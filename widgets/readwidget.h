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

#ifndef READWIDGET_H
#define READWIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include "speedreadertext.h"
#include <QWidget>

class ReadWidget : public QWidget {
    Q_OBJECT

    public:
        ReadWidget();
        ~ReadWidget();

        void prepareForReading();

        QLabel* getLabel();
        QPushButton* getStartStopButton();
        QPushButton* getCancelButton();
        QProgressBar* getProgressBar();

        void retranslate();

        void setStartStopButtonTextStart();
        void setStartStopButtonTextStop();
        void setStartStopButtonTextRestart();

        void setLabelTextFinished();
    private:
        QLabel*         mLabel;
        QPushButton*    mStartStopButton;
        QPushButton*    mCancelButton;
        QProgressBar*   mProgressBar;

        const char* mStartStopButtonChar;
        const char* mLabelChar;

        void setStartStopButtonText();
        void setLabelText();
};

#endif // READWIDGET_H
