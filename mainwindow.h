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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QClipboard>
#include <QNetworkReply>
#include <QTranslator>
#include <QListWidgetItem>
#include <QSignalMapper>
#include <QPixmap>

#include <settingswindow.h>
#include "settings.h"
#include "speedreadertext.h"

#include <QNetworkAccessManager>

#include <lib/VNTRSSReader/vntrssreader.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
    void setReadingText(QString text);

public slots:
    void changed(QString text, int readTextInPercent, SpeedReaderText::SpeedReaderStatus status);

    void on_readButton_clicked();
    void on_goButton_clicked();

protected:
    void changeEvent(QEvent*);

protected slots:
    void slotLanguageChanged(QAction* action);

private slots:
    void changedSlot(QClipboard::Mode mode);
    void replyFinished(QNetworkReply* networkReply);

    void updatedSettings();
    void itemClickedTimeout();
    void checkBoxClicked(QString id);
    void rssWebViewDialogReadSelectedText(QString text);
    void rssWebViewDialogGoSelectedText(QString text);
    void loadedRSS(QList<VNTRSSChannel*> rssChannels);

    void on_actionCheckForUpdates_triggered();
    void on_actionDonate_triggered();
    void on_actionReportABug_triggered();
    void on_actionSendFeedback_triggered();
    void on_actionSpeedReader_triggered();
    void on_actionAboutSpeedReader_triggered();
    void on_actionForkMeOnGitHub_triggered();

    void on_startStopButton_clicked();
    void on_cancelButton_clicked();
    void on_addRSSUrlButton_clicked();
    void on_refreshAllButton_clicked();
    void on_showRSSCheckBox_clicked(bool checked);
    void on_rssListWidget_itemClicked(QListWidgetItem* listWidgetItem);
    void on_rssListWidget_itemDoubleClicked(QListWidgetItem* listWidgetItem);

private:
    SpeedReaderText* mSpeedReaderText;
    Settings*        mSettings;
    SettingsWindow*  mSettingsWindow;

    Ui::MainWindow* mUI;
    QClipboard* mClipBoard;

    QNetworkAccessManager* mNetworkAccessManager;

    void updateStatusWidget();
    void updateReadLabel();
    void updateSpeedReaderText(QString text);

    // RSS
    QPixmap mRefreshPixmap;
    QListWidgetItem* mSingleClickedItem;
    bool mDoubleClicked;

    QSignalMapper* mCheckBoxSignalMapper;
    QMultiMap<QString, QListWidgetItem*> mIdListWidgetItemMultiMap;

    VNTRSSReader*   mRSSReader;

    void refreshRSSFeeds();
    void addRSSFeedDefaultEntry();
    QListWidgetItem* getListWidgetItem(QString title, QString description, QUrl url, QIcon icon);
    void addListWidgetItem(QString key, QString title, QString description, QUrl url, QImage image);

    // Language
    QTranslator mTranslator;
    QString     mCurrLang;

    void loadLanguage(const QString& language);
    void createLanguageMenu();
};

#endif // MAINWINDOW_H
