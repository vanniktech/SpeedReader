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

#include <QNetworkRequest>
#include <QNetworkProxyQuery>
#include <QNetworkProxy>

#include <settings.h>

#ifdef Q_OS_WIN32
const QString OS = "Windows";
#elif defined Q_OS_MAC
const QString OS = "Mac";
#elif defined Q_OS_ANDROID
const QString OS = "Android";
#elif defined Q_OS_LINUX
const QString OS = "Linux";
#endif

const QString coSite = "http://www.vanniktech.de/";
const QString coSiteSpeedReader = coSite + "SpeedReader/";
const QString coSiteDonate = coSite + "Donate/";
const QString coSiteSpeedReaderLatestOS = coSiteSpeedReader + "latest/" + OS;
const QString coSiteSpeedReaderLatestOSTXT = coSiteSpeedReaderLatestOS + ".txt";
const QString coSiteSpeedReaderLatestOSZIP = coSiteSpeedReaderLatestOS + ".zip";

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), mUI(new Ui::MainWindow) {
    mUI->setupUi(this);

    mSettings = Settings::getInstance();
    mSettingsWindow = new SettingsWindow(this);

    // Widgets
    mPasteWidget = new PasteWidget(coSiteDonate);
    QObject::connect(mPasteWidget->getReadButton(), SIGNAL(clicked()), this, SLOT(pasteWidgetReadButtonClicked()));
    QObject::connect(mPasteWidget->getGoButton(), SIGNAL(clicked()), this, SLOT(pasteWidgetGoButtonClicked()));
    QObject::connect(mPasteWidget->getShowRSSCheckBox(), SIGNAL(clicked(bool)), this, SLOT(pasteWidgetShowRSSCheckBoxClicked(bool)));

    mRSSWidget = new RSSWidget();
    QObject::connect(mRSSWidget, SIGNAL(rssListItemPressed(QString)), this, SLOT(rssListItemPressed(QString)));

    if (mSettings->getRSSUrls().size() == 0) {
        mPasteWidget->getShowRSSCheckBox()->clicked(false);
    } else {
        mPasteWidget->getShowRSSCheckBox()->setChecked(true);
    }

    mReadWidget = new ReadWidget();
    mReadWidget->hide();
    QObject::connect(mReadWidget->getStartStopButton(), SIGNAL(clicked()), this, SLOT(readWidgetStartStopButtonClicked()));
    QObject::connect(mReadWidget->getCancelButton(), SIGNAL(clicked()), this, SLOT(readWidgetCancelButtonClicked()));

    mStatusWidget = new StatusWidget();
    mStatusWidget->hide();

    // Settings
    mSpeedReaderText = 0;
    Settings* settings = Settings::getInstance();
    QObject::connect(settings, SIGNAL(updatedSettings()), this, SLOT(updatedSettings()));

    this->updateReadWidgetLabel();

    // Put all widgets together
    QBoxLayout* boxLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    boxLayout->addWidget(mPasteWidget);
    boxLayout->addWidget(mRSSWidget);
    boxLayout->addWidget(mReadWidget);
    boxLayout->addWidget(mStatusWidget);

    mUI->centralWidget->setLayout(boxLayout);

    // Clipboard
    mClipBoard = QApplication::clipboard();
    QObject::connect(mClipBoard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(changedSlot(QClipboard::Mode)));
    this->changedSlot(QClipboard::Clipboard);

    // Network
    mNetworkAccessManager = new QNetworkAccessManager(this);
    QObject::connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    QNetworkProxyQuery networkProxyQuery(QUrl(coSite.toLower()));
    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(networkProxyQuery);

    foreach (QNetworkProxy proxy, listOfProxies) {
        mNetworkAccessManager->setProxy(proxy);
    }

    this->createLanguageMenu();
}

MainWindow::~MainWindow() {
    if (mSpeedReaderText != 0 && mSpeedReaderText->isRunning()) {
        mSpeedReaderText->exit(0);
        mSpeedReaderText->terminate();
    }

    delete mUI;
    delete mPasteWidget;
    delete mReadWidget;
    delete mStatusWidget;

    delete mSettingsWindow;
    delete mNetworkAccessManager;
    delete mSpeedReaderText;
}

void MainWindow::setReadWidgetText(QString text) {
    mPasteWidget->getPlainTextEdit()->setPlainText(text);
}

void MainWindow::changedSlot(QClipboard::Mode mode) {
    if (mode == QClipboard::Clipboard && mPasteWidget->isVisible()) {
        mPasteWidget->getPlainTextEdit()->setPlainText(mClipBoard->text(mode));
    }
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

void MainWindow::pasteWidgetReadButtonClicked() {
    QString text = mPasteWidget->getPlainTextEdit()->toPlainText();
    text = text.isEmpty() ? QApplication::clipboard()->text() : text.append(' ');

    this->updateSpeedReaderText(text);

    mPasteWidget->hide();
    mRSSWidget->hide();
    mReadWidget->show();
    mStatusWidget->show();
}

void MainWindow::pasteWidgetGoButtonClicked() {
    this->pasteWidgetReadButtonClicked();
    this->readWidgetStartStopButtonClicked();
}

void MainWindow::pasteWidgetShowRSSCheckBoxClicked(bool clicked) {
    mRSSWidget->setHidden(!clicked);
}

void MainWindow::rssListItemPressed(QString text) {
    mPasteWidget->getPlainTextEdit()->setPlainText(text);
}

void MainWindow::updateSpeedReaderText(QString text) {
    mSpeedReaderText = new SpeedReaderText(text);
    QObject::connect(mSpeedReaderText, SIGNAL(changed(QString, int, SpeedReaderText::SpeedReaderStatus)), this, SLOT(changed(QString, int, SpeedReaderText::SpeedReaderStatus)));
    mReadWidget->prepareForReading();
    mStatusWidget->update(mSpeedReaderText);
}

void MainWindow::readWidgetStartStopButtonClicked() {
    if(mSpeedReaderText->isReading()) {
        mSpeedReaderText->stopReading();
        mReadWidget->setStartStopButtonTextStart();
        mStatusWidget->update(mSpeedReaderText);
        mStatusWidget->show();
    } else {
        mStatusWidget->hide();
        mSpeedReaderText->continueReading();

        if(!mSpeedReaderText->isRunning()) {
            mSpeedReaderText->start();
        }

        mReadWidget->setStartStopButtonTextStop();
    }

    mUI->actionSpeedReader->setEnabled(!mSpeedReaderText->isReading());
}

void MainWindow::readWidgetCancelButtonClicked() {
    mSpeedReaderText->terminate();

    mReadWidget->hide();
    mStatusWidget->hide();
    mPasteWidget->show();
    if (mPasteWidget->getShowRSSCheckBox()->isChecked()) mRSSWidget->show();

    mUI->actionSpeedReader->setEnabled(true);
}

void MainWindow::changed(QString text, int readTextInPercent, SpeedReaderText::SpeedReaderStatus status) {
    mReadWidget->getLabel()->setText(text);
    mReadWidget->getProgressBar()->setValue(readTextInPercent);

    if(status == SpeedReaderText::STOPPING) {
        this->readWidgetStartStopButtonClicked();
    } else if(status == SpeedReaderText::FINISHED) {
        if (mSettings->isJumpBackToStart()) {
            this->readWidgetCancelButtonClicked();
        } else {
            mReadWidget->setLabelTextFinished();
            mReadWidget->setStartStopButtonTextRestart();

            mStatusWidget->update(mSpeedReaderText);
            mStatusWidget->show();

            mUI->actionSpeedReader->setEnabled(true);
            mSpeedReaderText->resetReading();
        }
    }
}

void MainWindow::updatedSettings() {
    this->updateReadWidgetLabel();

    if (mSettings->changedRSSUrls()) {
        mRSSWidget->refresh();

        bool notEmpty = mSettings->getRSSUrls().size() != 0;
        mPasteWidget->getShowRSSCheckBox()->clicked(notEmpty);
        mPasteWidget->getShowRSSCheckBox()->setChecked(notEmpty);
    }

    if (mSettings->changedNumberOfWords() || mSettings->changedWordsPerMinute() || mSettings->changedNumberGrouping() || mSettings->changedWords()) {
        if (!mReadWidget->isVisible() || mSpeedReaderText == 0) return;

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

void MainWindow::updateReadWidgetLabel() {
    mReadWidget->getLabel()->setFont(QFont(mSettings->getFontFamily(), mSettings->getFontSize()));
    mReadWidget->getLabel()->setStyleSheet("QLabel { background-color: " + mSettings->getTextBackgroundColor().name() + "; color: " + mSettings->getTextColor().name() + "}");
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
    if(0 != action) {
        this->loadLanguage(action->data().toString());
    }
}

void switchTranslator(QTranslator& translator, const QString& filename) {
    qApp->removeTranslator(&translator);

    if(translator.load(filename, INT_LANG_DIR)) {
        qApp->installTranslator(&translator);
    }
}

void MainWindow::loadLanguage(const QString& language) {
    if(mCurrLang != language) {
        mCurrLang = language;
        QLocale locale = QLocale(mCurrLang);
        QLocale::setDefault(locale);
        switchTranslator(mTranslator, QString("%1.qm").arg(language));
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if(0 != event) {
        switch(event->type()) {
            case QEvent::LanguageChange: // this event is send if a translator is loaded
                mUI->retranslateUi(this);
                this->retranslateWidgets();
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

void MainWindow::retranslateWidgets() {
    mPasteWidget->retranslate();
    mReadWidget->retranslate();
    mStatusWidget->retranslate();
    mRSSWidget->retranslate();

    mSettingsWindow->retranslate();
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
