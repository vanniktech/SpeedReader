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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

#include <settings.h>
#include <QColorDialog>
#include <QFrame>

#include <QCloseEvent>

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QDialog {
    Q_OBJECT

public:
    SettingsWindow(QWidget* parent);
    ~SettingsWindow();

    void retranslate();
    void showEvent(QShowEvent* event);

protected:
    void closeEvent(QCloseEvent* closeEvent);
    void done(int r);

private slots:
    void on_changeTextColorPushButton_clicked();
    void on_changeTextBackgroundColorPushButton_clicked();
    void on_wordAddButton_clicked();
    void on_wordRemoveButton_clicked();
    void on_rssUrlRemoveButton_clicked();
    void on_rssUrlAddButton_clicked();
    void on_radioButtonNoHTTPNetworkProxy_clicked();
    void on_radioButtonUseSystemHTTPNetworkProxyConfiguration_clicked();
    void on_radioButtonCustomHTTPNetworkProxy_clicked();
    void on_displayLongerWordsCheckBox_clicked(bool checked);

private:
    Ui::SettingsWindow* mUI;
    Settings* mSettings;
    bool mSynchronized;

    void synchronizeWithSettings();
    void changeBackground(QFrame* frame, QColor backgroundColor);
    void addTableRow(Word word);
    QColor changeColorButtonClicked(QColor initialColor, QFrame* frame);
    void setCustomHTTPNetworkProxyInputsEnabled(bool enabled);
};

#endif // SETTINGSWINDOW_H
