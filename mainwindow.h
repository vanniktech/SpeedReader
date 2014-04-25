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

#include <settingswindow.h>
#include "settings.h"
#include "speedreadertext.h"

#include "widgets/pastewidget.h"
#include "widgets/readwidget.h"
#include "widgets/statuswidget.h"
#include "widgets/rsswidget.h"

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
    void setReadWidgetText(QString text);

public slots:
    void changed(QString text, int readTextInPercent, SpeedReaderText::SpeedReaderStatus status);

    void pasteWidgetGoButtonClicked();
    void pasteWidgetReadButtonClicked();

protected:
    void changeEvent(QEvent*);

protected slots:
    void slotLanguageChanged(QAction* action);

private slots:
    void changedSlot(QClipboard::Mode mode);
    void replyFinished(QNetworkReply* networkReply);

    void pasteWidgetShowRSSCheckBoxClicked(bool clicked);

    void rssListItemPressed(QString text);

    void readWidgetStartStopButtonClicked();
    void readWidgetCancelButtonClicked();

    void on_actionCheckForUpdates_triggered();
    void on_actionDonate_triggered();
    void on_actionReportABug_triggered();
    void on_actionSendFeedback_triggered();
    void on_actionSpeedReader_triggered();
    void on_actionAboutSpeedReader_triggered();

    void updatedSettings();

    void on_actionForkMeOnGitHub_triggered();

private:
    PasteWidget*    mPasteWidget;
    ReadWidget*     mReadWidget;
    StatusWidget*   mStatusWidget;
    RSSWidget*      mRSSWidget;

    SpeedReaderText*            mSpeedReaderText;
    Settings*        mSettings;
    SettingsWindow*  mSettingsWindow;

    Ui::MainWindow* mUI;
    QClipboard* mClipBoard;

    QNetworkAccessManager* mNetworkAccessManager;

    void updateReadWidgetLabel();
    void updateSpeedReaderText(QString text);

    // Language
    QTranslator     mTranslator;
    QString         mCurrLang;

    void loadLanguage(const QString& language);
    void createLanguageMenu();
    void retranslateWidgets();
};

#endif // MAINWINDOW_H
