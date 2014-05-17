/*
    Copyright 2014 Vanniktech - Niklas Baudy

    This file is part of VNTRSSReader.

    VNTRSSReader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    VNTRSSReader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VNTRSSReader. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VNTRSSREADER_H
#define VNTRSSREADER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>

#include <QUrl>
#include <QList>

#include "vntrsschannel.h"

class VNTRSSReader : public QObject {
    Q_OBJECT

public:
    explicit VNTRSSReader(QObject *parent = 0);
    ~VNTRSSReader();

    void load(QUrl url);
    void load(QUrl url, bool loadImages);
    void load(QList<QUrl> urls);
    void load(QList<QUrl> urls, bool loadImages);

private slots:
    void replyFinished(QNetworkReply* networkReply);
    void replyFinishedImages(QNetworkReply* networkReply);

private:
    void fireEmitIfDone();
    void loadImage(VNTRSSCommon* common);

    QMultiMap<QUrl, VNTRSSCommon*> mUrlItemMultiMap;
    QList<VNTRSSChannel*> mRSSChannels;
    bool mLoadImages;

    QNetworkAccessManager* mNetworkAccessManager;
    QNetworkAccessManager* mNetworkAccessManagerImages;

signals:
    void loadedRSS(QList<VNTRSSChannel*> rssChannels);
};

#endif // VNTRSSREADER_H
