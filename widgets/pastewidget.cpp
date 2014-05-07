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

#include "pastewidget.h"
#include <QVBoxLayout>

PasteWidget::PasteWidget(const QString coSiteDonate) : QWidget() {
    mCoSiteDonate = coSiteDonate;

    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0,0,0,0);

    mPlainTextEdit = new QPlainTextEdit();
    vBoxLayout->addWidget(mPlainTextEdit);

    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    hBoxLayout->setAlignment(Qt::AlignLeft);

    mReadButton = new QPushButton();
    hBoxLayout->addWidget(mReadButton);

    mGoButton = new QPushButton();
    hBoxLayout->addWidget(mGoButton);

    mShowRSSCheckBox = new QCheckBox();
    hBoxLayout->addWidget(mShowRSSCheckBox);

    vBoxLayout->addLayout(hBoxLayout);

    mDonateLabel = new QLabel();
    mDonateLabel->setOpenExternalLinks(true);
    mDonateLabel->setTextFormat(Qt::RichText);
    vBoxLayout->addWidget(mDonateLabel);

    this->retranslate();
    this->setLayout(vBoxLayout);
}

PasteWidget::~PasteWidget() {
    delete mPlainTextEdit;
    delete mGoButton;
    delete mReadButton;
    delete mDonateLabel;
    delete mShowRSSCheckBox;
}

QPlainTextEdit* PasteWidget::getPlainTextEdit() {
    return mPlainTextEdit;
}

QPushButton* PasteWidget::getReadButton() {
    return mReadButton;
}

QPushButton* PasteWidget::getGoButton() {
    return mGoButton;
}

QCheckBox* PasteWidget::getShowRSSCheckBox() {
    return mShowRSSCheckBox;
}

void PasteWidget::retranslate() {
    mGoButton->setText(tr("Go"));
    mReadButton->setText(tr("Read"));
    mShowRSSCheckBox->setText(tr("Show RSS feeds"));

    mDonateLabel->setText(QString("<h2><a href=\"%1\">%2</a></h2>").arg(mCoSiteDonate, tr("Donate")));
}
