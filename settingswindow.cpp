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
    mUI->displayLongerWordsCheckBox->setChecked(mSettings->displayLongerWordsLonger());
    this->on_displayLongerWordsCheckBox_clicked(mSettings->displayLongerWordsLonger());
    mUI->wordLengthSpinBox->setValue(mSettings->getWordLength());
    mUI->fontSizeSpinBox->setValue(mSettings->getFontSize());
    mUI->fontFamilyComboBox->setCurrentText(mSettings->getFontFamily());
    mUI->numberGroupingCheckBox->setChecked(mSettings->numberGrouping());
    mUI->jumpBackToStartCheckBox->setChecked(mSettings->jumpBackToStart());
    mUI->stallAtIndentionsCheckBox->setChecked(mSettings->stallAtIndentions());

    this->changeBackground(mUI->textColorFrame, mSettings->getTextColor());
    this->changeBackground(mUI->textBackgroundColorFrame, mSettings->getTextBackgroundColor());

    QList<Word> words = mSettings->getWords();
    mUI->wordsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    foreach (Word word, words) this->addTableRow(word);

    switch (mSettings->getHTTPNetworkProxyType()) {
        case Settings::NO_HTTP_NETWORK_PROXY:
            mUI->radioButtonNoHTTPNetworkProxy->setChecked(true);
            this->on_radioButtonNoHTTPNetworkProxy_clicked();
            break;
        case Settings::USE_SYSTEM_HTTP_NETWORK_PROXY_CONFIGURATION:
            mUI->radioButtonUseSystemHTTPNetworkProxyConfiguration->setChecked(true);
            this->on_radioButtonUseSystemHTTPNetworkProxyConfiguration_clicked();
            break;
        case Settings::CUSTOM_HTTP_NETWORK_PROXY:
            mUI->radioButtonCustomHTTPNetworkProxy->setChecked(true);
            this->on_radioButtonCustomHTTPNetworkProxy_clicked();
            break;
    }

    mUI->httpNetworkProxyServerLineEdit->setText(mSettings->getHTTPNetworkProxy().hostName());
    mUI->httpNetworkProxyPortLineEdit->setValidator(new QIntValidator(1, 65535));
    mUI->httpNetworkProxyPortLineEdit->setText(QString::number(mSettings->getHTTPNetworkProxy().port()));
}

SettingsWindow::~SettingsWindow() {
    delete mUI;
}

void SettingsWindow::showEvent(QShowEvent* event) {
    Q_UNUSED(event);

    mSynchronized = false;

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

    for (int i = 0; i < mUI->wordsTableWidget->rowCount(); i++) if (mUI->wordsTableWidget->item(i, 0)->text() == word) return;

    this->addTableRow(Word(word, false, false, 0));
    mUI->wordLineEdit->setText("");
}

void SettingsWindow::on_wordRemoveButton_clicked() {
    mUI->wordsTableWidget->removeRow(mUI->wordsTableWidget->currentRow());
}

void SettingsWindow::on_rssUrlAddButton_clicked() {
    QString value = mUI->rssUrlLineEdit->text();

    if (value.trimmed().length() == 0) return;

    QUrl url = QUrl::fromUserInput(value);

    for (int i = 0; i < mUI->rssListWidget->count(); i++) if (mUI->rssListWidget->item(i)->text() == url.toString()) return;

    mUI->rssListWidget->addItem(url.toString());
    mUI->rssUrlLineEdit->setText("");
}

void SettingsWindow::on_rssUrlRemoveButton_clicked() {
    qDeleteAll(mUI->rssListWidget->selectedItems());
}

void SettingsWindow::done(int r) {
    QDialog::done(r);
    if (!mSynchronized) this->synchronizeWithSettings();
}

void SettingsWindow::closeEvent(QCloseEvent* closeEvent) {
    if (!mSynchronized) this->synchronizeWithSettings();
    closeEvent->accept();
}

void SettingsWindow::synchronizeWithSettings() {
    mSynchronized = true;

    QList<Word> words;

    for (int i = 0; i < mUI->wordsTableWidget->rowCount(); i++) {
        Word word = Word(mUI->wordsTableWidget->item(i, 0)->text(), mUI->wordsTableWidget->item(i, 1)->checkState() == Qt::Checked, mUI->wordsTableWidget->item(i, 2)->checkState() == Qt::Checked, qMax(0, mUI->wordsTableWidget->item(i, 3)->text().toInt()));
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
    mSettings->setDisplayLongerWordsLonger(mUI->displayLongerWordsCheckBox->isChecked());
    mSettings->setWordLength(mUI->wordLengthSpinBox->value());
    mSettings->setNumberOfWords(mUI->numberOfWordsSpinBox->value());
    mSettings->setWordsPerMinute(mUI->wordsPerMinuteSpinBox->value());
    mSettings->setNumberGrouping(mUI->numberGroupingCheckBox->isChecked());
    mSettings->setJumpBackToStart(mUI->jumpBackToStartCheckBox->isChecked());
    mSettings->setStallAtIndentions(mUI->stallAtIndentionsCheckBox->isChecked());

    if (mUI->radioButtonNoHTTPNetworkProxy->isChecked()) {
        mSettings->setHTTPNetworkProxyType(Settings::NO_HTTP_NETWORK_PROXY);
    } else if (mUI->radioButtonUseSystemHTTPNetworkProxyConfiguration->isChecked()) {
        mSettings->setHTTPNetworkProxyType(Settings::USE_SYSTEM_HTTP_NETWORK_PROXY_CONFIGURATION);
    } else if (mUI->radioButtonCustomHTTPNetworkProxy->isChecked()) {
        if (mSettings->getHTTPNetworkProxyType() != Settings::CUSTOM_HTTP_NETWORK_PROXY) mSettings->setHTTPNetworkProxyType(Settings::CUSTOM_HTTP_NETWORK_PROXY);

        QNetworkProxy httpNetworkProxy;
        httpNetworkProxy.setType(QNetworkProxy::HttpProxy);
        httpNetworkProxy.setHostName(mUI->httpNetworkProxyServerLineEdit->text());
        httpNetworkProxy.setPort(mUI->httpNetworkProxyPortLineEdit->text().toInt());

        if (mSettings->getHTTPNetworkProxy().hostName() != httpNetworkProxy.hostName() || mSettings->getHTTPNetworkProxy().port() != httpNetworkProxy.port()) mSettings->setHTTPNetworkProxy(httpNetworkProxy);
    }

    mSettings->synchronize();
}

void SettingsWindow::changeBackground(QFrame* frame, QColor backgroundColor) {
    frame->setStyleSheet("QFrame { background-color: " + backgroundColor.name() + "}");
}

void SettingsWindow::addTableRow(Word word) {
    int rowCount = mUI->wordsTableWidget->rowCount();
    mUI->wordsTableWidget->insertRow(rowCount);

    QTableWidgetItem* item = new QTableWidgetItem(word.word);
    mUI->wordsTableWidget->setItem(rowCount, 0, item);

    QTableWidgetItem* stopWordWidgetItem = new QTableWidgetItem();
    stopWordWidgetItem->setCheckState(word.stopWord ? Qt::Checked : Qt::Unchecked);
    stopWordWidgetItem->setFlags(item->flags() ^ Qt::ItemIsEditable);
    mUI->wordsTableWidget->setItem(rowCount, 1, stopWordWidgetItem);

    QTableWidgetItem* breakWordWidgetItem = new QTableWidgetItem();
    breakWordWidgetItem->setCheckState(word.breakWord ? Qt::Checked : Qt::Unchecked);
    breakWordWidgetItem->setFlags(item->flags() ^ Qt::ItemIsEditable);
    mUI->wordsTableWidget->setItem(rowCount, 2, breakWordWidgetItem);

    QTableWidgetItem* delayWordWidgetItem = new QTableWidgetItem(QString::number(word.delayWord));
    mUI->wordsTableWidget->setItem(rowCount, 3, delayWordWidgetItem);
}

void SettingsWindow::on_radioButtonNoHTTPNetworkProxy_clicked() {
    this->setCustomHTTPNetworkProxyInputsEnabled(false);
}

void SettingsWindow::on_radioButtonUseSystemHTTPNetworkProxyConfiguration_clicked() {
    this->setCustomHTTPNetworkProxyInputsEnabled(false);
}

void SettingsWindow::on_radioButtonCustomHTTPNetworkProxy_clicked() {
    this->setCustomHTTPNetworkProxyInputsEnabled(true);
}

void SettingsWindow::setCustomHTTPNetworkProxyInputsEnabled(bool enabled) {
    mUI->httpNetworkProxyPortLineEdit->setEnabled(enabled);
    mUI->httpNetworkProxyServerLineEdit->setEnabled(enabled);
}

void SettingsWindow::on_displayLongerWordsCheckBox_clicked(bool checked) {
    mUI->wordLengthSpinBox->setEnabled(checked);
}
