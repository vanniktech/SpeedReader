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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>

#include "settingswindow.h"
#include "rsswebviewdialog.h"
#include "settings.h"
#include "speedreadertext.h"
#include "checkforupdates.h"
#include "rss.h"
#include "navigationdrawer.h"
#include "i18n.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void setSpeedReadingText(QString text);
    void addRSSSite(QUrl url);

public slots:
    void on_speedreadButton_clicked();

protected:
    void changeEvent(QEvent* event);
    void closeEvent(QCloseEvent *event);

protected slots:
    void slotLanguageChanged(QAction* action);

private slots:
    void changed(QString text, int speedreadTextInPercent, SpeedReaderText::SpeedReaderStatus status);
    void changedSlot(QClipboard::Mode mode);

    void updatedSettings();
    void failedToOpenDatabase();
    void rssWebViewDialogSpeedReadSelectedText(QString text);
    void rssWebViewDialogClosed();
    void on_rssListWidget_itemSelectionChanged();
    void rssDataChanged();
    void speedreadText(QString text);
    void openInWebView(QUrl url);
    void openInDefaultWebBrowser(QUrl url);
    void failedToLoadRSSChannel(QString errorMessage);

    void on_stopButton_clicked();
    void on_cancelButton_clicked();
    void on_continueButton_clicked();
    void on_plainTextEdit_textChanged();

    // Actions
    void on_actionCheckForUpdates_triggered();
    void on_actionDonate_triggered();
    void on_actionReportABug_triggered();
    void on_actionSendFeedback_triggered();
    void on_actionSpeedReader_triggered();
    void on_actionAboutSpeedReader_triggered();
    void on_actionForkMeOnGitHub_triggered();

    // Navigation Drawer
    void speedreadClicked();
    void allClicked();
    void unspeedreadClicked();
    void todayClicked();
    void yesterdayClicked();
    void rssSiteClicked(QString rssSite);
    void addRSSSiteButton_clicked();
    void refreshButton_clicked();

    // Shortcuts
    void speedReadFromClipBoardShortcut();
    void sShortcut();
    void cShortcut();
    void rShortcut();
    void aShortcut();
    void escapeShortcut();

private:
    SpeedReaderText  mSpeedReaderText;
    Settings*        mSettings;
    SettingsWindow*  mSettingsWindow;

    Ui::MainWindow* mUI;
    QRectF mOldTextBoundaries;
    void updateSpeedReadLabelText(QString value);

    QClipboard* mClipBoard;

    void updateStatusWidget();
    void updateSpeedReaderText(QString text);

    CheckForUpdates* mCheckForUpdates;

    NavigationDrawer* mNavigationDrawer;
    void refreshNavigationDrawer();

    RSS* mRSS;
    RSSWebViewDialog*mRSSWebViewDialog;
    void showRSSListView(RSSSelection rssSelection, QUrl rssSelectionRSSSite = QUrl());

    I18N* mI18N;
    void createLanguageMenu();
};

#endif // MAINWINDOW_H
