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

#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include "speedreadertext.h"
#include <QTextEdit>
#include <QLabel>
#include <QWidget>

class StatusWidget : public QWidget {
    Q_OBJECT

    public:
        StatusWidget();
        ~StatusWidget();

        void update(SpeedReaderText* speedReaderText);

        void retranslate();
    private:
        QTextEdit*  mTextEdit;
        QLabel*     mSegmentsStatusLabel;
        QLabel*     mStopWordsStatusLabel;
        QLabel*     mEstimatedReadingTime;

        QString mFormattedTime;
        QString mSegmentsStatusValue;
        QString mStopWordsStatusValue;
};

#endif // STATUSWIDGET_H
