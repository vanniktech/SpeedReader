/*
    Copyright 2014-2015 Vanniktech - Niklas Baudy

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

#include <QImageReader>

#include "vntrssreader.h"

#include "vntrsshandler.h"
#include "vntatomhandler.h"

VNTRSSReader::VNTRSSReader(QObject *parent) : QObject(parent) {
    mNetworkAccessManager = new QNetworkAccessManager(this);
    QObject::connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    mNetworkAccessManagerImages = new QNetworkAccessManager(this);
    QObject::connect(mNetworkAccessManagerImages, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedImages(QNetworkReply*)));

    mMissingChannels = 0;
}

VNTRSSReader::~VNTRSSReader() {
    delete mNetworkAccessManager;
    delete mNetworkAccessManagerImages;
}

void VNTRSSReader::load(const QUrl &url) {
    this->load(url, true);
}

void VNTRSSReader::load(const QUrl &url, const bool &loadImages) {
    mLoadImages = loadImages;
    mMissingChannels++;

    this->addInitialInputRSSUrlToRedirects(url);

    mNetworkAccessManager->get(QNetworkRequest(url));
}

void VNTRSSReader::load(const QList<QUrl> &urls) {
    this->load(urls, true);
}

void VNTRSSReader::load(const QList<QUrl> &urls, const bool &loadImages) {
    mLoadImages = loadImages;
    mMissingChannels += urls.size();

    foreach (QUrl url, urls) {
        this->addInitialInputRSSUrlToRedirects(url);
        mNetworkAccessManager->get(QNetworkRequest(url));
    }
}

void VNTRSSReader::replyFinished(QNetworkReply* networkReply) {
    mMissingChannels--;

    const bool networkError = networkReply->error();
    const QUrl networkReplyUrl = networkReply->url();

    if (!networkError) {
        const QUrl redirectUrl = networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

        if (!redirectUrl.isEmpty()) {
            foreach (QLinkedList<QUrl>* redirect, mRedirectUrls) {
                if (redirect->front() == networkReplyUrl) {
                    redirect->prepend(redirectUrl);
                    networkReply->deleteLater();
                    this->redirect(redirectUrl);
                    return;
                }
            }
        }
    }

    VNTRSSChannel* rssChannel;

    if (networkError) {
        rssChannel = new VNTRSSChannel();
        rssChannel->setErrorMessage(networkReplyUrl.toString() + " - " + networkReply->errorString());
    } else {
        rssChannel = this->parseData(networkReplyUrl.toString(), networkReply->readAll());
    }

    rssChannel->setRSSSite(networkReplyUrl);

    // Map the redirect urls back to the original one, if possible
    QList<QLinkedList<QUrl>* >::iterator it;

    for (it = mRedirectUrls.begin(); it != mRedirectUrls.end(); ++it) {
        QLinkedList<QUrl>* redirect = *it;

        if (redirect->first() == networkReplyUrl) {
            rssChannel->setRSSSite(redirect->last());
            mRedirectUrls.erase(it);
            delete redirect;
            break;
        }
    }

    foreach (VNTRSSItem* item, rssChannel->getRSSItems()) {
        this->loadImage(item);
    }

    mRSSChannels.append(rssChannel);
    this->loadImage(rssChannel);

    this->fireEmitIfDone();

    networkReply->deleteLater();
}

VNTRSSChannel* VNTRSSReader::parseData(const QString &origin, const QByteArray &data) const {
    QXmlStreamReader xml(data);
    VNTRSSChannel* rssChannel = new VNTRSSChannel();

    while (!xml.atEnd() && !xml.hasError()) {
        if (xml.isStartElement()) {
            VNTProtocolHandler* rssHandler = NULL;
            QString name = xml.name().toString();

            if (name == "rss") {
                rssHandler = new VNTRSSHandler();
            } else if (name == "feed") {
                rssHandler = new VNTAtomHandler();
            } else {
                rssChannel->setErrorMessage(tr("could_not_retrieve_a_valid_xml_response_from_%1").arg(origin));
            }

            if (rssHandler != NULL) {
                rssChannel = rssHandler->parseRSSChannel(xml);

                while (!xml.atEnd() && !xml.hasError()) {
                    if (xml.isStartElement() && xml.name() == rssHandler->getItemName()) {
                        rssChannel->addItem(rssHandler->parseRSSItem(xml));
                    }

                    if (!xml.atEnd()) {
                        xml.readNext();
                    }
                }

                delete rssHandler;
            }
        }

        if (!xml.atEnd()) {
            xml.readNext();
        }
    }

    if (xml.hasError()) {
        rssChannel->setErrorMessage(tr("invalid_xml_data_from_%1_error_message_%2").arg(origin, xml.errorString()));
        return rssChannel;
    }

    xml.clear();

    return rssChannel;
}

void VNTRSSReader::replyFinishedImages(QNetworkReply* networkReply) {
    QList<VNTRSSCommon*> commons = mUrlItemMultiMap.values(networkReply->url());
    QImage image = QImageReader(networkReply).read();

    foreach (VNTRSSCommon* common, commons) {
        common->setImage(image);
    }

    mUrlItemMultiMap.remove(networkReply->url());

    this->fireEmitIfDone();

    networkReply->deleteLater();
}

void VNTRSSReader::loadImage(VNTRSSCommon* common) {
    if (mLoadImages && !common->getImageUrl().isEmpty()) {
        if (mUrlItemMultiMap.values(common->getImageUrl()).size() == 0) {
            mNetworkAccessManagerImages->get(QNetworkRequest(common->getImageUrl()));
        }

        mUrlItemMultiMap.insert(common->getImageUrl(), common);
    }
}

void VNTRSSReader::fireEmitIfDone() {
    if (mUrlItemMultiMap.size() == 0 && mMissingChannels == 0) {
        emit loadedRSS(mRSSChannels);
        mRSSChannels.clear();
    }
}

void VNTRSSReader::addInitialInputRSSUrlToRedirects(const QUrl &url) {
    QLinkedList<QUrl>* redirect = new QLinkedList<QUrl>();
    redirect->prepend(url);
    mRedirectUrls.append(redirect);
}

void VNTRSSReader::redirect(const QUrl &url) {
    mMissingChannels += 1;
    mNetworkAccessManager->get(QNetworkRequest(url));
}
