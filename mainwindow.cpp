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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QBoxLayout>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTimer>

#include <QNetworkRequest>

#include <settings.h>
#include <lib/vntformatter.h>

#ifdef Q_OS_WIN32
const QString OS = "Windows";
#elif defined Q_OS_MAC
const QString OS = "Mac";
#elif defined Q_OS_ANDROID
const QString OS = "Android";
#elif defined Q_OS_LINUX
const QString OS = "Linux";
#endif

const QString coSite = ORGANIZATION_DOMAIN;
const QString coSiteDonate = coSite + "Donate/";
const QString coSiteSpeedReader = coSite + "SpeedReader/";
const QString coSiteSpeedReaderLatestOS = coSiteSpeedReader + "latest/" + OS;
const QString coSiteSpeedReaderLatestOSTXT = coSiteSpeedReaderLatestOS + ".txt";
const QString coSiteSpeedReaderLatestOSZIP = coSiteSpeedReaderLatestOS + ".zip";

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), mUI(new Ui::MainWindow) {
    mUI->setupUi(this);

    mSpeedReaderText = 0;

    mSettings = Settings::getInstance();
    QObject::connect(mSettings, SIGNAL(updatedSettings()), this, SLOT(updatedSettings()));
    mSettingsWindow = new SettingsWindow(this);

    // RSS
    mRefreshPixmap = QPixmap(":/images/refresh.png");
    mCheckBoxSignalMapper = new QSignalMapper(this);
    QObject::connect(mCheckBoxSignalMapper, SIGNAL(mapped(QString)), this, SLOT(checkBoxClicked(QString)));

    mRSSReader = new VNTRSSReader();
    QObject::connect(mRSSReader, SIGNAL(loadedRSS(QList<VNTRSSChannel*>)), this, SLOT(loadedRSS(QList<VNTRSSChannel*>)));

    if (mSettings->getRSSUrls().size() == 0) mUI->showRSSCheckBox->clicked(false);
    else mUI->showRSSCheckBox->setChecked(true);

    this->updateReadLabel();

    // Clipboard
    mClipBoard = QApplication::clipboard();
    QObject::connect(mClipBoard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(changedSlot(QClipboard::Mode)));
    this->changedSlot(QClipboard::Clipboard);

    // Network
    mNetworkAccessManager = new QNetworkAccessManager(this);
    QObject::connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    this->createLanguageMenu();
    this->updatedSettings();
}

MainWindow::~MainWindow() {
    if (mSpeedReaderText != 0 && mSpeedReaderText->isRunning()) {
        mSpeedReaderText->exit(0);
        mSpeedReaderText->terminate();
    }

    delete mUI;
    delete mSettings;
    delete mRSSReader;
    delete mSettingsWindow;
    delete mSpeedReaderText;
    delete mNetworkAccessManager;
    delete mCheckBoxSignalMapper;
}

void MainWindow::setReadingText(QString text) {
    mUI->plainTextEdit->setPlainText(text);
}

void MainWindow::changedSlot(QClipboard::Mode mode) {
    if (mode == QClipboard::Clipboard && mUI->pasteWidget->isVisible()) mUI->plainTextEdit->setPlainText(mClipBoard->text(mode));
}

void MainWindow::replyFinished(QNetworkReply* networkReply) {
    QString value = QString(networkReply->readAll());
    delete networkReply;

    if (value.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText(tr("Could not get information about latest version"));
        msgBox.exec();

        return;
    }

    if (value == APPLICATION_VERSION) {
        QMessageBox msgBox;
        msgBox.setText(tr("Your version is up to date"));
        msgBox.exec();
    } else if (value > APPLICATION_VERSION) {
        QMessageBox messageBox(tr("Check for updates"), tr("New version %1 is available. Do you want to download it?").arg(value), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape, QMessageBox::NoButton, this);
        messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
        messageBox.setButtonText(QMessageBox::No, tr("No"));

        if (messageBox.exec() == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl(coSiteSpeedReaderLatestOSZIP));
        }
    }
}

void MainWindow::rssWebViewDialogReadSelectedText(QString text) {
    mUI->plainTextEdit->setPlainText(text);
    this->on_readButton_clicked();
}

void MainWindow::rssWebViewDialogGoSelectedText(QString text) {
    mUI->plainTextEdit->setPlainText(text);
    this->on_goButton_clicked();
}

void MainWindow::updateSpeedReaderText(QString text) {
    mSpeedReaderText = new SpeedReaderText(text);
    QObject::connect(mSpeedReaderText, SIGNAL(changed(QString, int, SpeedReaderText::SpeedReaderStatus)), this, SLOT(changed(QString, int, SpeedReaderText::SpeedReaderStatus)));

    mUI->progressBar->setValue(0);
    mUI->startStopButton->setText(tr("Start"));
    mUI->label->setText(tr("Press start to start reading"));

    this->updateStatusWidget();
}

void MainWindow::changed(QString text, int readTextInPercent, SpeedReaderText::SpeedReaderStatus status) {
    mUI->label->setText(text);
    mUI->progressBar->setValue(readTextInPercent);

    if (status == SpeedReaderText::STOPPING) {
        this->on_startStopButton_clicked();
    } else if (status == SpeedReaderText::FINISHED) {
        if (mSettings->jumpBackToStart()) {
            this->on_cancelButton_clicked();
        } else {
            mUI->label->setText(tr("Finished"));
            mUI->startStopButton->setText(tr("Restart"));

            this->updateStatusWidget();
            mUI->statusWidget->show();

            mUI->actionSpeedReader->setEnabled(true);
            mSpeedReaderText->resetReading();
        }
    }
}

void MainWindow::updatedSettings() {
    this->updateReadLabel();

    if (mSettings->changedHTTPNetworkProxy()) {
        switch (mSettings->getHTTPNetworkProxyType()) {
            case Settings::NO_HTTP_NETWORK_PROXY:
                QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
                break;
            case Settings::USE_SYSTEM_HTTP_NETWORK_PROXY_CONFIGURATION:
                QNetworkProxyFactory::setUseSystemConfiguration(true);
                break;
            case Settings::CUSTOM_HTTP_NETWORK_PROXY:
                QNetworkProxy::setApplicationProxy(mSettings->getHTTPNetworkProxy());
                break;
        }

        if (!mSettings->changedRSSUrls()) this->refreshRSSFeeds();
    }

    if (mSettings->changedRSSUrls()) {
        this->refreshRSSFeeds();

        bool notEmpty = mSettings->getRSSUrls().size() != 0;
        mUI->showRSSCheckBox->clicked(notEmpty);
        mUI->showRSSCheckBox->setChecked(notEmpty);
    }

    if (mSettings->changedNumberOfWords() || mSettings->changedWordsPerMinute() || mSettings->changedNumberGrouping() || mSettings->changedWords() || mSettings->changedDisplayLongerWordsLonger() || mSettings->changedStallAtIndentions()) {
        if (!mUI->readWidget->isVisible() || mSpeedReaderText == 0) return;

        QMessageBox messageBox(tr("Changed settings"), tr("You have changed some important reading settings. How do you want to proceed?"), QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape | QMessageBox::Default, this);
        messageBox.setButtonText(QMessageBox::Yes, tr("Restart completely"));
        messageBox.setButtonText(QMessageBox::No, tr("Restart, but start with current word"));
        messageBox.setButtonText(QMessageBox::Cancel, tr("Keep it as is"));

        switch (messageBox.exec()) {
            case QMessageBox::Yes:
                this->updateSpeedReaderText(mSpeedReaderText->getText());
                break;
            case QMessageBox::No:
                this->updateSpeedReaderText(mSpeedReaderText->getRemainingText());
                break;
            case QMessageBox::Cancel:
                break;
        }
    }
}

void MainWindow::updateReadLabel() {
    mUI->label->setFont(QFont(mSettings->getFontFamily(), mSettings->getFontSize()));
    mUI->label->setStyleSheet("QLabel { background-color: " + mSettings->getTextBackgroundColor().name() + "; color: " + mSettings->getTextColor().name() + "}");
}

// Language stuff
const QString INT_LANG_DIR = ":/languages/";

void MainWindow::createLanguageMenu() {
    QActionGroup* langGroup = new QActionGroup(mUI->menuLanguage);
    langGroup->setExclusive(true);

    QObject::connect(langGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotLanguageChanged(QAction*)));

    QString defaultLocale = QLocale::system().name();
    defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

    QDir dir(INT_LANG_DIR);
    QStringList fileNames = dir.entryList(QStringList("*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.truncate(locale.lastIndexOf('.'));

        QString lang = QLocale::languageToString(QLocale(locale).language());
        QIcon ico(QString("%1%2.png").arg(INT_LANG_DIR).arg(locale));

        QAction* action = new QAction(ico, lang, this);
        action->setCheckable(true);
        action->setData(locale);

        mUI->menuLanguage->addAction(action);
        langGroup->addAction(action);

        if (defaultLocale == locale) {
            action->setChecked(true);
            this->slotLanguageChanged(action);
        }
    }
}

void MainWindow::slotLanguageChanged(QAction* action) {
    if (0 != action) this->loadLanguage(action->data().toString());
}

void switchTranslator(QTranslator& translator, const QString& filename) {
    qApp->removeTranslator(&translator);

    if (translator.load(filename, INT_LANG_DIR)) qApp->installTranslator(&translator);
}

void MainWindow::loadLanguage(const QString& language) {
    if (mCurrLang != language) {
        mCurrLang = language;
        QLocale locale = QLocale(mCurrLang);
        QLocale::setDefault(locale);
        switchTranslator(mTranslator, QString("%1.qm").arg(language));
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if (0 != event) {
        switch(event->type()) {
            case QEvent::LanguageChange: // this event is send if a translator is loaded
                mUI->retranslateUi(this);
                mSettingsWindow->retranslate();
                mUI->donateLabel->setText(QString("<h2><a href=\"%1\">%2</a></h2>").arg(coSiteDonate, tr("Donate")));
                break;
            case QEvent::LocaleChange: { // this event is send, if the system language changes
                    QString locale = QLocale::system().name();
                    locale.truncate(locale.lastIndexOf('_'));
                    loadLanguage(locale);
                }
                break;
            default:
                break;
        }
    }

    QMainWindow::changeEvent(event);
}

// Menu
void MainWindow::on_actionCheckForUpdates_triggered() {
    mNetworkAccessManager->get(QNetworkRequest(QUrl(coSiteSpeedReaderLatestOSTXT)));
}

void MainWindow::on_actionDonate_triggered() {
    QDesktopServices::openUrl(QUrl(coSiteDonate));
}

void MainWindow::on_actionReportABug_triggered() {
    QDesktopServices::openUrl(QUrl("mailto:info@vanniktech.de?subject=[Bug report] - SpeedReader (" + OS + ")"));
}

void MainWindow::on_actionSendFeedback_triggered() {
    QDesktopServices::openUrl(QUrl("mailto:info@vanniktech.de?subject=[Feedback] - SpeedReader (" + OS + ")"));
}

void MainWindow::on_actionSpeedReader_triggered() {
    mSettingsWindow->show();
    mSettingsWindow->raise();
    mSettingsWindow->activateWindow();
}

void MainWindow::on_actionAboutSpeedReader_triggered() {
    QString text = QString("SpeedReader %1<br />%2 Vanniktech<br />Niklas Baudy<br /><a href=\"%3\">%4</a>").arg(APPLICATION_VERSION, tr("Published by"), coSiteSpeedReader, tr("More infos"));

    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(text);
    msgBox.exec();
}

void MainWindow::on_actionForkMeOnGitHub_triggered() {
    QDesktopServices::openUrl(QUrl("https://github.com/vanniktech/SpeedReader"));
}

// Slots
void MainWindow::on_startStopButton_clicked() {
    if (mSpeedReaderText->isReading()) {
        mSpeedReaderText->stopReading();
        mUI->startStopButton->setText(tr("Start"));
        this->updateStatusWidget();
        mUI->statusWidget->show();
    } else {
        mUI->statusWidget->hide();
        mSpeedReaderText->continueReading();

        if (!mSpeedReaderText->isRunning()) {
            mSpeedReaderText->start();
        }

        mUI->startStopButton->setText(tr("Stop"));
    }

    mUI->actionSpeedReader->setEnabled(!mSpeedReaderText->isReading());
}

void MainWindow::on_cancelButton_clicked() {
    mSpeedReaderText->terminate();

    mUI->stackedWidget->setCurrentIndex(0);
    mUI->rssWidget->setVisible(mUI->showRSSCheckBox->isChecked());

    mUI->actionSpeedReader->setEnabled(true);
}

void MainWindow::on_readButton_clicked() {
    QString text = mUI->plainTextEdit->toPlainText();
    if (text.isEmpty()) text = QApplication::clipboard()->text();

    this->updateSpeedReaderText(text);

    mUI->stackedWidget->setCurrentIndex(1);
    mUI->statusWidget->setVisible(true);
}

void MainWindow::on_goButton_clicked() {
    this->on_readButton_clicked();
    this->on_startStopButton_clicked();
}

void MainWindow::on_addRSSUrlButton_clicked() {
    QUrl url = QUrl::fromUserInput(mUI->addRSSUrlLineEdit->text());

    if (mSettings->appendRSSUrl(url)) {
        mUI->addRSSUrlLineEdit->clear();
        mRSSReader->load(url);
    }
}

void MainWindow::on_refreshAllButton_clicked() {
    this->refreshRSSFeeds();
}

void MainWindow::on_showRSSCheckBox_clicked(bool checked) {
    mUI->rssWidget->setHidden(!checked);
}

void MainWindow::on_rssListWidget_itemClicked(QListWidgetItem* listWidgetItem) {
    if (!mDoubleClicked) {
        QTimer::singleShot(300, this, SLOT(itemClickedTimeout()));
        mSingleClickedItem = listWidgetItem;
    }
}

void MainWindow::itemClickedTimeout() {
    if (!mDoubleClicked) mUI->plainTextEdit->setPlainText(mSingleClickedItem->text());
    else mDoubleClicked = false;
}

void MainWindow::on_rssListWidget_itemDoubleClicked(QListWidgetItem* listWidgetItem) {
    mDoubleClicked = true;

    QUrl url = QUrl(listWidgetItem->data(Qt::UserRole).toString());
    QDesktopServices::openUrl(url);
}

void MainWindow::updateStatusWidget() {
    mUI->textEdit->setHtml(mSpeedReaderText->getHTMLColoredText());
    mUI->textEdit->scrollToAnchor("myAnchorForCurrentWord");

    mUI->segmentsStatusLabel->setText(QString("%1 / %2 %3").arg(QString::number(mSpeedReaderText->getIndex()), QString::number(mSpeedReaderText->getMaxIndex()), tr("segments read")));
    mUI->stopWordsStatusLabel->setText(QString("%1 / %2 %3").arg(QString::number(mSpeedReaderText->getStopWordsRead()), QString::number(mSpeedReaderText->getStopWordsCount()), tr("stopwords read")));
    mUI->estimatedRemainingReadingTimeLabel->setText(QString("%1 %2").arg(tr("Estimated reamining reading time:"), VNTFormatter::formatMilliseconds(mSpeedReaderText->getEstimatedTimeInMS())));
}

void MainWindow::refreshRSSFeeds() {
    mUI->rssListWidget->clear();
    mIdListWidgetItemMultiMap.clear();

    while (QLayoutItem* item = mUI->checkBoxRSSLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    QList<QUrl> rssUrls = mSettings->getRSSUrls();

    if (rssUrls.size() == 0) this->addRSSFeedDefaultEntry();
    else mRSSReader->load(rssUrls);
}

void MainWindow::addRSSFeedDefaultEntry() {
    mUI->rssListWidget->addItem(this->getListWidgetItem(tr("Default entry"), tr("If you want, you can quite easily add some RSS feeds via the settings menu or the add button above."), QUrl("http://vanniktech.de/"), QIcon(":/icon.png")));
}

QListWidgetItem* MainWindow::getListWidgetItem(QString title, QString description, QUrl url, QIcon icon) {
    QListWidgetItem* listWidgetItem = new QListWidgetItem(QString("%1\n%2").arg(title, description));
    listWidgetItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    listWidgetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    listWidgetItem->setIcon(icon);
    listWidgetItem->setData(Qt::UserRole, url);

    return listWidgetItem;
}

void MainWindow::addListWidgetItem(QString key, QString title, QString description, QUrl url, QImage image) {
    QListWidgetItem* listWidgetItem = this->getListWidgetItem(title, description, url, QPixmap::fromImage(image));
    mUI->rssListWidget->addItem(listWidgetItem);
    mIdListWidgetItemMultiMap.insert(key, listWidgetItem);
}

void MainWindow::checkBoxClicked(QString id) {
    QList<QListWidgetItem*> listWidgets = mIdListWidgetItemMultiMap.values(id);

    foreach (QListWidgetItem* item, listWidgets) item->setHidden(!item->isHidden());
}

void MainWindow::loadedRSS(QList<VNTRSSChannel*> rssChannels) {
    foreach (VNTRSSChannel* rssChannel, rssChannels) {
        if (rssChannel->hasError()) {
            QMessageBox::warning(this, tr("Loading RSS feed"), rssChannel->getErrorMessage());
            delete rssChannel;
            continue;
        } else if (rssChannel->getTitle().isEmpty() || rssChannel->getRSSUrl().isEmpty()) {
            delete rssChannel;
            continue;
        }

        QString key = QString::number(qrand());
        this->addListWidgetItem(key, rssChannel->getTitle(), rssChannel->getPlainDescription(), rssChannel->getRSSUrl(), rssChannel->getImage());

        QList<VNTRSSItem*> items = rssChannel->getItems();
        foreach (VNTRSSItem* item, items) this->addListWidgetItem(key, item->getTitle(), item->getPlainDescription(), item->getGuid() == "" ? item->getLink() : item->getGuid(), item->getImage());

        QCheckBox* checkBox = new QCheckBox(rssChannel->getTitle());
        checkBox->setChecked(true);
        mUI->checkBoxRSSLayout->addWidget(checkBox);

        QObject::connect(checkBox, SIGNAL(stateChanged(int)), mCheckBoxSignalMapper, SLOT(map()));
        mCheckBoxSignalMapper->setMapping(checkBox, key);

        delete rssChannel;
    }
}
