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

#ifndef RSSWIDGET_H
#define RSSWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QListWidget>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QHash>
#include <QSignalMapper>
#include <QMultiMap>
#include <QPushButton>
#include <QLineEdit>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

#include <lib/VNTRSSReader/vntrssreader.h>
#include <lib/VNTRSSReader/vntrsschannel.h>

#include <settings.h>

class RSSWidget : public QWidget {
    Q_OBJECT

public:
    RSSWidget();
    void refresh();
    QListWidget* getRSSListWidget();
    ~RSSWidget();
    void retranslate();

public slots:
    void loadedRSS(VNTRSSChannel* rssChannel, QString errorMessage);

signals:
    void rssListItemPressed(QString text);

private:
    QListWidgetItem* getListWidgetItem(QString title, QString description, QUrl url);
    void addListWidgetItem(QString key, QString title, QString description, QUrl url, QUrl imageUrl);

    QListWidget* mRSSListWidget;
    QNetworkAccessManager* mNetworkAccessManager;
    QMultiMap<QUrl, QListWidgetItem*> mUrlListWidgetItemHash;
    QMultiMap<QString, QListWidgetItem*> mIdListWidgetItemMultiMap;
    QMultiMap<QString, QWidget*> mIdRefreshWidgetsMultiMap;

    QHBoxLayout* mCheckBoxRSSVLayout;
    QSignalMapper* mCheckBoxSignalMapper;
    QSignalMapper* mCheckBoxRefreshSignalMapper;
    VNTRSSReader* mRSSReader;
    Settings* mSettings;

    QPixmap mRefreshPixmap;

    QLineEdit* mAddRSSUrlLineEdit;
    QPushButton* mAddRSSUrlPushButton;

    QListWidgetItem* mSingleClickedItem;
    bool mDoubleClicked;

    void addDefaultEntry();

private slots:
    void replyFinished(QNetworkReply* networkReply);
    void checkBoxClicked(QString id);
    void checkBoxRefreshClicked(QString id);
    void itemPressed(QListWidgetItem* listWidgetItem);
    void itemDoubleClicked(QListWidgetItem* listWidgetItem);
    void itemClickedTimeout();
    void addRSSUrlPushButtonClicked();
};

#endif // RSSWIDGET_H
