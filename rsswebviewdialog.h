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

#ifndef RSSWEBVIEWDIALOG_H
#define RSSWEBVIEWDIALOG_H

#include <QDialog>

namespace Ui {
    class RSSWebViewDialog;
}

class RSSWebViewDialog : public QDialog {
    Q_OBJECT

signals:
    void speedreadSelectedText(QString text);
    void closed();

public:
    explicit RSSWebViewDialog(QWidget *parent = 0);
    ~RSSWebViewDialog();

    void retranslate();
    void load(QUrl url);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_speedreadPushButton_clicked();
    void on_reloadButton_clicked();

private:
    Ui::RSSWebViewDialog *mUI;
};

#endif // RSSWEBVIEWDIALOG_H
