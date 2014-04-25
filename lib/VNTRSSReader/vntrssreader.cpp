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

#include "vntrssreader.h"

#include <QXmlStreamReader>

VNTRSSReader::VNTRSSReader(QObject *parent) : QObject(parent) {
    mNetworkAccessManager = new QNetworkAccessManager(this);
    QObject::connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

VNTRSSReader::~VNTRSSReader() {
    delete mNetworkAccessManager;
}

void VNTRSSReader::load(QUrl url, bool systemProxy = false) {
    mNetworkAccessManager->setProxy(QNetworkProxy());

    if (systemProxy) {
        QNetworkProxyQuery networkProxyQuery(url);
        QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(networkProxyQuery);

        foreach (QNetworkProxy proxy, listOfProxies) {
            mNetworkAccessManager->setProxy(proxy);
        }
    }

    mNetworkAccessManager->get(QNetworkRequest(url));
}

void VNTRSSReader::load(QUrl url, QNetworkProxy proxy) {
    mNetworkAccessManager->setProxy(proxy);
    mNetworkAccessManager->get(QNetworkRequest(url));
}

void VNTRSSReader::replyFinished(QNetworkReply* networkReply) {
    QXmlStreamReader xmlReader(networkReply->readAll());
    QString iLink, iTitle, iDescription, iPubDate, iCategory, iGuid, iImageUrl;
    QString cLink, cTitle, cDescription, cPubDate, cLanguage, cCopyright, cImageUrl;
    QString name, rssVersion, errorMessage;
    bool didBeginProcessingItems = false;

    QList<VNTRSSItem> items;
    for (int i = 0; !xmlReader.atEnd() && !xmlReader.hasError();) {
        xmlReader.readNext();

        if (xmlReader.columnNumber() == 0 && xmlReader.hasError()) {
            errorMessage = tr("Could not retrieve a valid XML response from %1").arg(networkReply->url().toString());
            break;
        }

        name = xmlReader.name().toString();

        if (xmlReader.isStartElement()) {
            if (i++ == 0) { // Root Element
                if (name != "rss") {
                    errorMessage = QString("%1 %2").arg(networkReply->url().toString(), tr("is not a valid RSS feed"));
                    break;
                } else {
                    rssVersion = xmlReader.attributes().value("version").toString().simplified();

                    if (rssVersion != "2.0") {
                        errorMessage = tr("Unsupported RSS version %1 in RSS feed %2").arg(rssVersion, networkReply->url().toString());
                        break;
                    }
                }
            }

            // Common
            if (name == "link" && didBeginProcessingItems) iLink = xmlReader.readElementText(); else if (name == "link" && cLink.isNull()) cLink = xmlReader.readElementText();
            if (name == "title" && didBeginProcessingItems) iTitle = xmlReader.readElementText(); else if (name == "title" && cTitle.isNull()) cTitle = xmlReader.readElementText();
            if (name == "description" && didBeginProcessingItems) iDescription = xmlReader.readElementText(); else if (name == "description" && cDescription.isNull()) cDescription = xmlReader.readElementText();
            if (name == "pubDate" && didBeginProcessingItems) iPubDate = xmlReader.readElementText(); else if (name == "pubDate" && cPubDate.isNull()) cPubDate = xmlReader.readElementText();

            // Item only
            if (name == "category" && didBeginProcessingItems) iCategory = xmlReader.readElementText();
            if (name == "guid" && didBeginProcessingItems) iGuid = xmlReader.readElementText();
            if (name == "content" && didBeginProcessingItems) iImageUrl = xmlReader.attributes().value("url").toString();

            // Channel only
            if (name == "language" && cLanguage.isNull()) cLanguage = xmlReader.readElementText();
            if (name == "copyright" && cCopyright.isNull()) cCopyright = xmlReader.readElementText();
            if (name == "url" && cImageUrl.isNull()) cImageUrl = xmlReader.readElementText();

            if (name == "item") didBeginProcessingItems = true;
        } else if (xmlReader.isEndElement()) {
            VNTRSSItem item(iLink, iTitle, iDescription, iPubDate, iCategory, iGuid, iImageUrl);

            if (name == "item" && !rssVersion.isNull()) items.append(item);
        }
    }

    VNTRSSChannel* rssChannel = new VNTRSSChannel(cLink, cTitle, cDescription, cPubDate, cLanguage, cCopyright, cImageUrl, networkReply->url(), items);

    emit loadedRSS(rssChannel, errorMessage);

    delete networkReply;
}
