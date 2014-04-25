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

#ifndef PASTEWIDGET_H
#define PASTEWIDGET_H

#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QCheckBox>

class PasteWidget : public QWidget {
    Q_OBJECT

    public:
        PasteWidget(const QString coSiteDonate);
        ~PasteWidget();

        QPlainTextEdit* getPlainTextEdit();
        QPushButton* getReadButton();
        QPushButton* getGoButton();
        QCheckBox* getShowRSSCheckBox();

        void retranslate();

    private:
        QPlainTextEdit* mPlainTextEdit;
        QPushButton*    mReadButton;
        QPushButton*    mGoButton;
        QCheckBox*      mShowRSSCheckBox;
        QLabel*         mDonateLabel;

        QString         mCoSiteDonate;
};

#endif // PASTEWIDGET_H
