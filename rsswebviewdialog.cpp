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

#include <QShortcut>

#include "rsswebviewdialog.h"
#include "ui_rsswebviewdialog.h"

RSSWebViewDialog::RSSWebViewDialog(QWidget *parent) : QDialog(parent), mUI(new Ui::RSSWebViewDialog) {
    mUI->setupUi(this);
    mUI->horizontalLayout->setAlignment(Qt::AlignLeft);

    QShortcut *sShortcut = new QShortcut(QKeySequence("S"), this);
    QObject::connect(sShortcut, SIGNAL(activated()), this, SLOT(on_speedreadPushButton_clicked()));

    QShortcut *rShortcut = new QShortcut(QKeySequence("R"), this);
    QObject::connect(rShortcut, SIGNAL(activated()), this, SLOT(on_reloadButton_clicked()));
}

RSSWebViewDialog::~RSSWebViewDialog() {
    delete mUI;
}

void RSSWebViewDialog::retranslate() {
    mUI->retranslateUi(this);
}

void RSSWebViewDialog::load(QUrl url) {
    mUI->webView->load(url);
}

void RSSWebViewDialog::on_speedreadPushButton_clicked() {
    QString selectedText = mUI->webView->selectedText();

    if (!selectedText.isEmpty()) {
        emit speedreadSelectedText(mUI->webView->selectedText());
        this->close();
    }
}

void RSSWebViewDialog::closeEvent(QCloseEvent *event) {
    emit closed();

    QDialog::closeEvent(event);
}

void RSSWebViewDialog::on_reloadButton_clicked() {
    mUI->webView->reload();
}
