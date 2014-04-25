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

#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QFontDatabase>

SettingsWindow::SettingsWindow(QWidget* parent) : QDialog(parent), mUI(new Ui::SettingsWindow) {
    mUI->setupUi(this);

    mSettings = Settings::getInstance();

    QStringList families = QFontDatabase().families();

    foreach (QString family, families) {
        mUI->fontFamilyComboBox->addItem(family);
    }

    mUI->numberOfWordsSpinBox->setValue(mSettings->getNumberOfWords());
    mUI->wordsPerMinuteSpinBox->setValue(mSettings->getWordsPerMinute());
    mUI->fontSizeSpinBox->setValue(mSettings->getFontSize());
    mUI->fontFamilyComboBox->setCurrentText(mSettings->getFontFamily());
    mUI->numberGroupingCheckBox->setChecked(mSettings->isNumberGrouping());
    mUI->jumpBackToStartCheckBox->setChecked(mSettings->isJumpBackToStart());

    this->changeBackground(mUI->textColorFrame, mSettings->getTextColor());
    this->changeBackground(mUI->textBackgroundColorFrame, mSettings->getTextBackgroundColor());

    QList<Word> words = mSettings->getWords();
    mUI->wordsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    foreach (Word word, words) {
        this->addTableRow(word.word, word.stopWord, word.breakWord);
    }
}

SettingsWindow::~SettingsWindow() {
    delete mUI;
}

void SettingsWindow::showEvent(QShowEvent* event) {
    Q_UNUSED(event);

    QList<QUrl> rssUrls = mSettings->getRSSUrls();
    mUI->rssListWidget->clear();

    foreach (QUrl url, rssUrls) {
        mUI->rssListWidget->addItem(url.toString());
    }
}

void SettingsWindow::retranslate() {
    mUI->retranslateUi(this);
}

void SettingsWindow::on_changeTextColorPushButton_clicked() {
    mSettings->setTextColor(this->changeColorButtonClicked(mSettings->getTextColor(), mUI->textColorFrame));
}

void SettingsWindow::on_changeTextBackgroundColorPushButton_clicked() {
    mSettings->setTextBackgroundColor(this->changeColorButtonClicked(mSettings->getTextBackgroundColor(), mUI->textBackgroundColorFrame));
}

QColor SettingsWindow::changeColorButtonClicked(QColor initialColor, QFrame* frame) {
    QColor color = QColorDialog::getColor(initialColor, this);

    this->changeBackground(frame, color);
    this->raise();
    this->activateWindow();

    return color;
}

void SettingsWindow::on_wordAddButton_clicked() {
    QString word = mUI->wordLineEdit->text();

    if (word.trimmed().length() == 0) return;

    for (int i = 0; i < mUI->wordsTableWidget->rowCount(); i++) {
        if (mUI->wordsTableWidget->item(i, 0)->text() == word) return;
    }

    this->addTableRow(word, false, false);
    mUI->wordLineEdit->setText("");
}

void SettingsWindow::on_wordRemoveButton_clicked() {
    mUI->wordsTableWidget->removeRow(mUI->wordsTableWidget->currentRow());
}

void SettingsWindow::on_rssUrlAddButton_clicked() {
    QString value = mUI->rssUrlLineEdit->text();

    if (value.trimmed().length() == 0) return;

    QUrl url = QUrl::fromUserInput(value);

    for (int i = 0; i < mUI->rssListWidget->count(); i++) {
        if (mUI->rssListWidget->item(i)->text() == url.toString()) return;
    }

    mUI->rssListWidget->addItem(url.toString());
    mUI->rssUrlLineEdit->setText("");
}

void SettingsWindow::on_rssUrlRemoveButton_clicked() {
    qDeleteAll(mUI->rssListWidget->selectedItems());
}

void SettingsWindow::on_SettingsWindow_finished(int result) {
    Q_UNUSED(result);

    QList<Word> words;

    for (int i = 0; i < mUI->wordsTableWidget->rowCount(); i++) {
        Word word = Word(mUI->wordsTableWidget->item(i, 0)->text(), mUI->wordsTableWidget->item(i, 1)->checkState() == Qt::Checked, mUI->wordsTableWidget->item(i, 2)->checkState() == Qt::Checked);
        words.append(word);
    }

    mSettings->setWords(words);

    QList<QUrl> rssUrls;

    for (int i = 0; i < mUI->rssListWidget->count(); ++i) {
        rssUrls.append(QUrl::fromUserInput(mUI->rssListWidget->item(i)->text()));
    }

    mSettings->setRSSUrls(rssUrls);

    mSettings->setFontFamily(mUI->fontFamilyComboBox->currentText());
    mSettings->setFontSize(mUI->fontSizeSpinBox->value());
    mSettings->setNumberOfWords(mUI->numberOfWordsSpinBox->value());
    mSettings->setWordsPerMinute(mUI->wordsPerMinuteSpinBox->value());
    mSettings->setNumberGrouping(mUI->numberGroupingCheckBox->isChecked());
    mSettings->setJumpBackToStart(mUI->jumpBackToStartCheckBox->isChecked());

    mSettings->synchronize();

    this->close();
}

void SettingsWindow::changeBackground(QFrame* frame, QColor backgroundColor) {
    frame->setStyleSheet("QFrame { background-color: " + backgroundColor.name() + "}");
}

void SettingsWindow::addTableRow(QString word, bool stopWord, bool breakWord) {
    int rowCount = mUI->wordsTableWidget->rowCount();
    mUI->wordsTableWidget->insertRow(rowCount);

    QTableWidgetItem* item = new QTableWidgetItem(word);
    mUI->wordsTableWidget->setItem(rowCount, 0, item);

    QTableWidgetItem* stopWordWidgetItem = new QTableWidgetItem();
    stopWordWidgetItem->setCheckState(stopWord ? Qt::Checked : Qt::Unchecked);
    stopWordWidgetItem->setFlags(item->flags() ^ Qt::ItemIsEditable);
    mUI->wordsTableWidget->setItem(rowCount, 1, stopWordWidgetItem);

    QTableWidgetItem* breakWordWidgetItem = new QTableWidgetItem();
    breakWordWidgetItem->setCheckState(breakWord ? Qt::Checked : Qt::Unchecked);
    breakWordWidgetItem->setFlags(item->flags() ^ Qt::ItemIsEditable);
    mUI->wordsTableWidget->setItem(rowCount, 2, breakWordWidgetItem);
}
