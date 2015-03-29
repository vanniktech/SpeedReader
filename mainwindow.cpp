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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QPainter>
#include <QShortcut>
#include <QInputDialog>

#include "lib/vntformatter.h"

#ifdef Q_OS_WIN32
const QString OS = "Windows";
#elif defined Q_OS_MAC
const QString OS = "Mac";
#elif defined Q_OS_LINUX
const QString OS = "Linux";
#endif

const QString coSite = ORGANIZATION_DOMAIN;
const QString coSiteGitHub = "https://github.com/vanniktech/SpeedReader";
const QString coSiteDonate = coSite + "Donate/";
const QString coSiteSpeedReader = coSite + "SpeedReader/";
const QString coSiteSpeedReaderOSTXT = coSiteSpeedReader + "latest/" + OS + ".txt";
const QString coSiteSpeedReaderOSZIP = coSiteSpeedReader + OS + "/" + APPLICATION_VERSION;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), mSpeedReaderText(Settings::getInstance(), this), mUI(new Ui::MainWindow) {
    mUI->setupUi(this);

    mI18N = new I18N(this);

    mNavigationDrawer = new NavigationDrawer(mUI->navigationDrawerScrollAreaWidget);
    QObject::connect(mNavigationDrawer, SIGNAL(onRefreshRSSSiteButtonClicked()), this, SLOT(refreshButton_clicked()));
    QObject::connect(mNavigationDrawer, SIGNAL(onAddRSSSiteButtonClicked()), this, SLOT(addRSSSiteButton_clicked()));
    QObject::connect(mNavigationDrawer, SIGNAL(onSpeedreadClicked()), this, SLOT(speedreadClicked()));
    QObject::connect(mNavigationDrawer, SIGNAL(onAllClicked()), this, SLOT(allClicked()));
    QObject::connect(mNavigationDrawer, SIGNAL(onUnspeedreadClicked()), this, SLOT(unspeedreadClicked()));
    QObject::connect(mNavigationDrawer, SIGNAL(onTodayClicked()), this, SLOT(todayClicked()));
    QObject::connect(mNavigationDrawer, SIGNAL(onYesterdayClicked()), this, SLOT(yesterdayClicked()));
    QObject::connect(mNavigationDrawer, SIGNAL(rssSiteClicked(QString)), this, SLOT(rssSiteClicked(QString)));

    mRSS = new RSS(mUI->rssListWidget, this);
    QObject::connect(mRSS, SIGNAL(failedToOpenDatabase()), this, SLOT(failedToOpenDatabase()));
    QObject::connect(mRSS, SIGNAL(rssDataChanged()), this, SLOT(rssDataChanged()));
    QObject::connect(mRSS, SIGNAL(speedread(QString)), this, SLOT(speedreadText(QString)));
    QObject::connect(mRSS, SIGNAL(openInWebView(QUrl)), this, SLOT(openInWebView(QUrl)));
    QObject::connect(mRSS, SIGNAL(openInDefaultWebBrowser(QUrl)), this, SLOT(openInDefaultWebBrowser(QUrl)));
    QObject::connect(mRSS, SIGNAL(failedToLoadRSSChannel(QString)), this, SLOT(failedToLoadRSSChannel(QString)));

    // Some layout fixes (can't be done through Qt Designer)
    mUI->buttonLayout->setAlignment(Qt::AlignLeft);
    mUI->stopCancelButtonLayout->setAlignment(Qt::AlignLeft);
    mUI->continueButton->setVisible(false);

    this->speedreadClicked();
    this->refreshNavigationDrawer();

    QObject::connect(&mSpeedReaderText, SIGNAL(changed(QString, int, SpeedReaderText::SpeedReaderStatus)), this, SLOT(changed(QString, int, SpeedReaderText::SpeedReaderStatus)));
    this->updateStatusWidget();

    mSettings = Settings::getInstance();
    QObject::connect(mSettings, SIGNAL(updatedSettings()), this, SLOT(updatedSettings()));
    mSettingsWindow = new SettingsWindow(this);
    QObject::connect(mSettingsWindow, SIGNAL(deleteRSSCache()), mRSS, SLOT(deleteRSSCache()));
    QObject::connect(mSettingsWindow, SIGNAL(deleteAllRSSSites()), mRSS, SLOT(deleteAllRSSSites()));
    QObject::connect(mSettingsWindow, SIGNAL(deleteRSSSites(QList<QString>)), mRSS, SLOT(deleteRSSSites(QList<QString>)));
    QObject::connect(mSettingsWindow, SIGNAL(addRSSSites(QList<QString>)), mRSS, SLOT(addRSSSites(QList<QString>)));

    mRSSWebViewDialog = new RSSWebViewDialog(this);
    QObject::connect(mRSSWebViewDialog, SIGNAL(speedreadSelectedText(QString)), this, SLOT(rssWebViewDialogSpeedReadSelectedText(QString)));
    QObject::connect(mRSSWebViewDialog, SIGNAL(closed()), this, SLOT(rssWebViewDialogClosed()));

    mClipBoard = QApplication::clipboard();
    QObject::connect(mClipBoard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(changedSlot(QClipboard::Mode)));
    this->changedSlot(QClipboard::Clipboard);

    mCheckForUpdates = new CheckForUpdates(coSiteSpeedReaderOSTXT, coSiteSpeedReaderOSZIP, this);
    mCheckForUpdates->checkForUpdates(true);

    this->createLanguageMenu();
    this->updatedSettings();

    #ifdef QT_DEBUG
        this->setFixedWidth(1100);
    #else
        this->restoreGeometry(mSettings->getMainWindowGeometry());
    #endif

    QShortcut *speedReadFromClipBoardShortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    QObject::connect(speedReadFromClipBoardShortcut, SIGNAL(activated()), this, SLOT(speedReadFromClipBoardShortcut()));

    QShortcut *sShortcut = new QShortcut(QKeySequence("S"), this);
    QObject::connect(sShortcut, SIGNAL(activated()), this, SLOT(sShortcut()));

    QShortcut *cShortcut = new QShortcut(QKeySequence("C"), this);
    QObject::connect(cShortcut, SIGNAL(activated()), this, SLOT(cShortcut()));

    QShortcut *rShortcut = new QShortcut(QKeySequence("R"), this);
    QObject::connect(rShortcut, SIGNAL(activated()), this, SLOT(rShortcut()));

    QShortcut *aShortcut = new QShortcut(QKeySequence("A"), this);
    QObject::connect(aShortcut, SIGNAL(activated()), this, SLOT(aShortcut()));

    QShortcut *escapeShortcut = new QShortcut(QKeySequence("Esc"), this);
    QObject::connect(escapeShortcut, SIGNAL(activated()), this, SLOT(escapeShortcut()));
}

MainWindow::~MainWindow() {
    delete mSettings;
}

void MainWindow::refreshNavigationDrawer() {
    mNavigationDrawer->update(mRSS->getChannels(), mRSS->unspeedreadAllCount(), mRSS->unspeedreadTodayCount(), mRSS->unspeedreadYesterdayCount());
}

void MainWindow::setSpeedReadingText(QString text) {
    mUI->plainTextEdit->setPlainText(text);
}

void MainWindow::changedSlot(QClipboard::Mode mode) {
    if (mode == QClipboard::Clipboard && mUI->pasteWidget->isVisible() && !mSettingsWindow->isVisible()) this->setSpeedReadingText(mClipBoard->text(mode));
}

void MainWindow::rssWebViewDialogSpeedReadSelectedText(QString text) {
    this->speedreadText(text);
}

void MainWindow::rssWebViewDialogClosed() {
    mSettings->saveRSSWebViewDialogGeometry(mRSSWebViewDialog->saveGeometry());
}

void MainWindow::updateSpeedReadLabelText(QString value) {
    QPixmap pixmap(mUI->label->width(), mUI->label->height());
    pixmap.fill(mSettings->getTextBackgroundColor());

    QPainter paint(&pixmap);

    QFont font(mSettings->getFontFamily());
    font.setPixelSize(mSettings->getFontSize());
    paint.setFont(font);
    paint.setPen(mSettings->getTextColor());

    QRectF textBoundaries;
    paint.drawText(mUI->label->rect(), Qt::AlignCenter, value, &textBoundaries);

    if (!value.isEmpty()) mOldTextBoundaries = textBoundaries;
    else textBoundaries = mOldTextBoundaries;

    const float size = 5.f;
    float x = (float) mUI->label->width() / 8.f;
    float width = (float) mUI->label->width() / 4.f * 3.f;
    paint.fillRect(x, textBoundaries.y() - size * 2, width, size, mSettings->getLinesColor());
    paint.fillRect(x, textBoundaries.y() + textBoundaries.height() + size * 2, width, size, mSettings->getLinesColor());

    mUI->label->setPixmap(pixmap);
}

void MainWindow::updateSpeedReaderText(QString text) {
    mSpeedReaderText.setText(text);
    this->updateStatusWidget();
}

void MainWindow::changed(QString text, int speedreadTextInPercent, SpeedReaderText::SpeedReaderStatus status) {
    if (status == SpeedReaderText::STOPPING) this->on_stopButton_clicked();
    else if (status == SpeedReaderText::FINISHED) this->on_cancelButton_clicked();
    else if (status == SpeedReaderText::SPEEDREADING) {
        this->updateSpeedReadLabelText(text);
        mUI->progressBar->setValue(speedreadTextInPercent);
    }
}

void MainWindow::updatedSettings() {
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

        this->refreshButton_clicked();
    }

    if (mSettings->changedRSSRefreshRate()) mRSS->startRefreshRSSTimer(mSettings->getRSSRefreshRate());
}

void MainWindow::createLanguageMenu() {
    QActionGroup* langGroup = new QActionGroup(mUI->menuLanguage);
    langGroup->setExclusive(true);

    QObject::connect(langGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotLanguageChanged(QAction*)));

    QList<Language> languages = mI18N->getLanguages();

    foreach (Language language, languages) {
        QAction* action = new QAction(QIcon(language.iconFilePath), language.name, langGroup);
        action->setCheckable(true);
        action->setData(language.locale);

        mUI->menuLanguage->addAction(action);

        if (language.isDefault) {
            action->setChecked(true);
            this->slotLanguageChanged(action);
        }
    }
}

void MainWindow::slotLanguageChanged(QAction* action) {
    if (0 != action) {
        QString locale = action->data().toString();
        mI18N->loadLanguage(locale);
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if (0 != event) {
        switch(event->type()) {
            case QEvent::LanguageChange: // this event is send if a translator is loaded (will be in I18N class)
                mUI->retranslateUi(this);
                mSettingsWindow->retranslate();
                mRSSWebViewDialog->retranslate();
                mUI->donateLabel->setText(QString("<h2><a href=\"%1\">%2</a></h2>").arg(coSiteDonate, tr("Donate")));
                mUI->googlePlayStoreLabel->setText(QString("<a href=\"%1\"><img src='data:image/png;base64,%2'></a>").arg(coSiteSpeedReader + "Android", mI18N->getBase64GooglePlayStoreIconForCurrentLanguage()));
                this->refreshNavigationDrawer();
                break;
            case QEvent::LocaleChange: // this event is send, if the system language changes
                mI18N->onSystemLocalChanged();
                break;
            default:
                break;
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    mSettings->setMainWindowGeometry(this->saveGeometry());

    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionCheckForUpdates_triggered() {
    mCheckForUpdates->checkForUpdates();
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
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(QString("SpeedReader %1<br>%2 Vanniktech<br>Copyright 2014 - 2015 Vanniktech - Niklas Baudy<br><a href=\"%3\">%4</a>").arg(APPLICATION_VERSION, tr("Published by"), coSiteSpeedReader, tr("More information")));
    msgBox.exec();
}

void MainWindow::on_actionForkMeOnGitHub_triggered() {
    QDesktopServices::openUrl(QUrl(coSiteGitHub));
}

void MainWindow::speedReadFromClipBoardShortcut() {
    this->setSpeedReadingText(QApplication::clipboard()->text());
    this->on_speedreadButton_clicked();
}

void MainWindow::sShortcut() {
    if (mUI->stackedWidget->currentIndex() == 0 && mUI->speedreadButton->isVisible()) this->on_speedreadButton_clicked();
    else if (mUI->stackedWidget->currentIndex() == 1 && mUI->stopButton->isVisible()) this->on_stopButton_clicked();
}

void MainWindow::cShortcut() {
    if (mUI->stackedWidget->currentIndex() == 0 && mUI->continueButton->isVisible()) this->on_continueButton_clicked();
    else if (mUI->stackedWidget->currentIndex() == 1 && mUI->cancelButton->isVisible()) this->on_cancelButton_clicked();
}

void MainWindow::rShortcut() {
    if (mUI->stackedWidget->currentIndex() == 0 && !mNavigationDrawer->isRefreshing()) this->refreshButton_clicked();
}

void MainWindow::aShortcut() {
    if (mUI->stackedWidget->currentIndex() == 0) this->addRSSSiteButton_clicked();
}

void MainWindow::escapeShortcut() {
    if (mUI->stackedWidget->currentIndex() == 1 && mUI->cancelButton->isVisible()) this->on_cancelButton_clicked();
}

void MainWindow::on_stopButton_clicked() {
    if (mSpeedReaderText.stopSpeedReading()) {
        mUI->continueButton->setVisible(true);
        mUI->speedreadButton->setVisible(false);
        mUI->stackedWidget->setCurrentIndex(0);
        this->speedreadClicked();
        this->updateStatusWidget();

        mUI->actionSpeedReader->setEnabled(true);
    }
}

void MainWindow::on_cancelButton_clicked() {
    mSpeedReaderText.terminate();

    mUI->stackedWidget->setCurrentIndex(0);

    mUI->actionSpeedReader->setEnabled(true);
    this->updateStatusWidget();
}

void MainWindow::on_continueButton_clicked() {
    mUI->stackedWidget->setCurrentIndex(1);
    mUI->continueButton->setVisible(false);
    mUI->speedreadButton->setVisible(true);

    mSpeedReaderText.continueSpeedReading();
}

void MainWindow::on_speedreadButton_clicked() {
    QString text = mUI->plainTextEdit->toPlainText();
    if (text.isEmpty()) text = QApplication::clipboard()->text();

    this->speedreadText(text);
}

void MainWindow::addRSSSiteButton_clicked() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("add_rss_site"), tr("insert_the_url_here"), QLineEdit::Normal, "", &ok);

    if (ok) {
        this->addRSSSite(QUrl::fromUserInput(text));
    }
}

void MainWindow::addRSSSite(QUrl url) {
    if (mSettings->appendRSSSite(url)) {
        mRSS->load(url);
    }
}

void MainWindow::refreshButton_clicked() {
    mNavigationDrawer->startRefreshAnimation();
    mRSS->loadAllRSSFeeds();
}

void MainWindow::on_rssListWidget_itemSelectionChanged() {
    mRSS->markRSSItemAsSpeedRead(mUI->rssListWidget->row(mUI->rssListWidget->currentItem()));
}

void MainWindow::on_plainTextEdit_textChanged() {
    QString plainText = mUI->plainTextEdit->toPlainText();

    if (plainText != mSpeedReaderText.getText()) {
        this->updateSpeedReaderText(mUI->plainTextEdit->toPlainText());
        mUI->continueButton->setVisible(false);
        mUI->speedreadButton->setVisible(true);
    }
}

void MainWindow::speedreadText(QString text) {
    this->updateSpeedReaderText(text);

    mUI->actionSpeedReader->setEnabled(false);
    mUI->stackedWidget->setCurrentIndex(1);
    mUI->progressBar->setValue(0);

    mSpeedReaderText.continueSpeedReading();
}

void MainWindow::updateStatusWidget() {
    mUI->textEdit->setHtml(mSpeedReaderText.getHTMLColoredText());
    mUI->textEdit->scrollToAnchor(SpeedReaderText::HTML_ANCHOR_CURRENT_WORD);

    mUI->segmentsStatusLabel->setText(QString("%1 / %2 %3").arg(QString::number(mSpeedReaderText.getIndex()), QString::number(mSpeedReaderText.getMaxIndex()), tr("segments SpeedRead")));
    mUI->stopWordsStatusLabel->setText(QString("%1 / %2 %3").arg(QString::number(mSpeedReaderText.getStopWordsRead()), QString::number(mSpeedReaderText.getStopWordsCount()), tr("stopwords SpeedRead")));
    mUI->estimatedRemainingSpeedReadingTimeLabel->setText(QString("%1 %2").arg(tr("Estimated remaining SpeedReading time:"), VNTFormatter::formatMilliseconds(mSpeedReaderText.getEstimatedTimeInMS())));
}

void MainWindow::openInWebView(QUrl url) {
    mRSSWebViewDialog->load(url);
    mRSSWebViewDialog->restoreGeometry(mSettings->getRSSWebViewDialogGeometry());
    mRSSWebViewDialog->show();
    mRSSWebViewDialog->raise();
    mRSSWebViewDialog->activateWindow();
}

void MainWindow::openInDefaultWebBrowser(QUrl url) {
    QDesktopServices::openUrl(url);
}

void MainWindow::rssDataChanged() {
    mNavigationDrawer->stopRefreshAnimation();
    this->refreshNavigationDrawer();
}

void MainWindow::failedToOpenDatabase() {
    QMessageBox::critical(this, tr("RSS database"), tr("Could not be loaded and opened."));
}

void MainWindow::failedToLoadRSSChannel(QString errorMessage) {
    QMessageBox::warning(this, tr("Loading RSS feed"), errorMessage);
}

void MainWindow::speedreadClicked() {
    mUI->rssWidget->hide();
    mUI->pasteWidget->show();
    mUI->statusWidget->show();

    mRSS->resetRSSSelectionData();
}

void MainWindow::allClicked() {
    if (mRSS->getRSSSelectionData().selection != ALL) this->showRSSListView(ALL);
}

void MainWindow::unspeedreadClicked() {
    if (mRSS->getRSSSelectionData().selection != UNSPEEDREAD) this->showRSSListView(UNSPEEDREAD);
}

void MainWindow::todayClicked() {
    if (mRSS->getRSSSelectionData().selection != TODAY) this->showRSSListView(TODAY);
}

void MainWindow::yesterdayClicked() {
    if (mRSS->getRSSSelectionData().selection != YESTERDAY) this->showRSSListView(YESTERDAY);
}

void MainWindow::rssSiteClicked(QString rssSite) {
    QUrl rss(rssSite);

    if (mRSS->getRSSSelectionData().selection != SITE || rss != mRSS->getRSSSelectionData().rssSite) this->showRSSListView(SITE, rss);
}

void MainWindow::showRSSListView(RSSSelection rssSelection, QUrl rssSelectionRSSSite) {
    mUI->rssWidget->show();
    mUI->pasteWidget->hide();
    mUI->statusWidget->hide();

    RSSSelectionData rssSelectionData;
    rssSelectionData.selection = rssSelection;
    rssSelectionData.rssSite = rssSelectionRSSSite;
    mRSS->refreshRSSList(rssSelectionData);
}
