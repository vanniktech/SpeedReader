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

#include "rsswidget.h"

#include <QListWidgetItem>
#include <QIcon>
#include <QHBoxLayout>
#include <QImageReader>
#include <QPixmap>
#include <QSize>
#include <QCheckBox>
#include <QLabel>
#include <QNetworkProxyQuery>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTimer>

#include <lib/vntlabel.h>

RSSWidget::RSSWidget() : QWidget() {
    mRSSListWidget = new QListWidget();
    mRSSListWidget->setIconSize(QSize(80, 80));
    mRSSListWidget->setWordWrap(true);
    QObject::connect(mRSSListWidget, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(itemPressed(QListWidgetItem*)));
    QObject::connect(mRSSListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));

    mCheckBoxRSSVLayout = new QHBoxLayout();
    mCheckBoxRSSVLayout->setAlignment(Qt::AlignLeft);

    QHBoxLayout* addRSSUrlLayout = new QHBoxLayout();
    mAddRSSUrlLineEdit = new QLineEdit();
    mAddRSSUrlLineEdit->setPlaceholderText("http://server.com/rss");
    mAddRSSUrlPushButton = new QPushButton(tr("Add"));
    QObject::connect(mAddRSSUrlPushButton, SIGNAL(clicked()), this, SLOT(addRSSUrlPushButtonClicked()));
    addRSSUrlLayout->addWidget(mAddRSSUrlLineEdit);
    addRSSUrlLayout->addWidget(mAddRSSUrlPushButton);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(addRSSUrlLayout);
    layout->addWidget(mRSSListWidget);
    layout->addLayout(mCheckBoxRSSVLayout);

    this->setLayout(layout);

    mRSSReader = new VNTRSSReader();
    QObject::connect(mRSSReader, SIGNAL(loadedRSS(VNTRSSChannel*, QString)), this, SLOT(loadedRSS(VNTRSSChannel*, QString)));

    mSettings = Settings::getInstance();

    mCheckBoxSignalMapper = new QSignalMapper(this);
    QObject::connect(mCheckBoxSignalMapper, SIGNAL(mapped(QString)), this, SLOT(checkBoxClicked(QString)));

    mCheckBoxRefreshSignalMapper = new QSignalMapper(this);
    QObject::connect(mCheckBoxRefreshSignalMapper, SIGNAL(mapped(QString)), this, SLOT(checkBoxRefreshClicked(QString)));

    mNetworkAccessManager = new QNetworkAccessManager(this);
    QObject::connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QNetworkProxyQuery networkProxyQuery(QUrl("http://www.google.com/"));
    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(networkProxyQuery);

    foreach (QNetworkProxy proxy, listOfProxies) {
        mNetworkAccessManager->setProxy(proxy);
    }

    mRefreshPixmap = QPixmap(":/images/refresh.png");

    this->refresh();
}

void RSSWidget::refresh() {
    mRSSListWidget->clear();
    mIdListWidgetItemMultiMap.clear();
    mIdRefreshWidgetsMultiMap.clear();
    mUrlListWidgetItemHash.clear();

    while (QLayoutItem* item = mCheckBoxRSSVLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    QList<QUrl> rssUrls = mSettings->getRSSUrls();

    if (rssUrls.size() == 0) {
        this->addDefaultEntry();
    }

    foreach (QUrl url, rssUrls) {
        mRSSReader->load(url, true);
    }
}

QListWidget* RSSWidget::getRSSListWidget() {
    return mRSSListWidget;
}

void RSSWidget::retranslate() {
    if (mSettings->getRSSUrls().size() == 0 && mRSSListWidget->count() == 1) {
        mRSSListWidget->takeItem(0);
        this->addDefaultEntry();
    }
}

RSSWidget::~RSSWidget() {
    delete mRSSListWidget;
    delete mNetworkAccessManager;
    delete mCheckBoxSignalMapper;
    delete mCheckBoxRefreshSignalMapper;
    delete mRSSReader;
    delete mSettings;
    delete mAddRSSUrlLineEdit;
    delete mAddRSSUrlPushButton;
    delete mCheckBoxRSSVLayout;
}

void RSSWidget::loadedRSS(VNTRSSChannel* rssChannel, QString errorMessage) {
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, tr("Loading RSS feed"), errorMessage);
        delete rssChannel;
        return;
    }

    QString key = QString::number(qrand());
    QList<VNTRSSItem> items = rssChannel->getItems();

    this->addListWidgetItem(key, rssChannel->getTitle(), rssChannel->getDescription(), rssChannel->getRSSUrl(), rssChannel->getImageUrl());

    foreach (VNTRSSItem item, items) {
        this->addListWidgetItem(key, item.getTitle(), item.getPlainDescription(), item.getGuid() == "" ? item.getLink() : item.getGuid(), item.getImageUrl());
    }

    QCheckBox* checkBox = new QCheckBox(rssChannel->getTitle());
    checkBox->setChecked(true);
    mCheckBoxRSSVLayout->addWidget(checkBox);
    mIdRefreshWidgetsMultiMap.insert(key, checkBox);

    QObject::connect(checkBox, SIGNAL(stateChanged(int)), mCheckBoxSignalMapper, SLOT(map()));
    mCheckBoxSignalMapper->setMapping(checkBox, key);

    VNTLabel* refreshLabel = new VNTLabel();
    refreshLabel->setPixmap(mRefreshPixmap);
    mCheckBoxRSSVLayout->addWidget(refreshLabel);
    mIdRefreshWidgetsMultiMap.insert(key, refreshLabel);

    QObject::connect(refreshLabel, SIGNAL(clicked()), mCheckBoxRefreshSignalMapper, SLOT(map()));
    mCheckBoxRefreshSignalMapper->setMapping(refreshLabel, key);

    delete rssChannel;
}

void RSSWidget::replyFinished(QNetworkReply* networkReply) {
    QList<QListWidgetItem*> listWidgetItems = mUrlListWidgetItemHash.values(networkReply->url());
    QPixmap icon = QPixmap::fromImage(QImageReader(networkReply).read());

    foreach (QListWidgetItem* item, listWidgetItems) {
        item->setIcon(icon);
    }

    mUrlListWidgetItemHash.remove(networkReply->url());

    delete networkReply;
}

void RSSWidget::checkBoxClicked(QString id) {
    QList<QListWidgetItem*> listWidgets = mIdListWidgetItemMultiMap.values(id);

    foreach (QListWidgetItem* item, listWidgets) {
        item->setHidden(!item->isHidden());
    }
}

void RSSWidget::checkBoxRefreshClicked(QString id) {
    QList<QListWidgetItem*> listWidgetItems = mIdListWidgetItemMultiMap.values(id);
    QList<QWidget*> widgetItems = mIdRefreshWidgetsMultiMap.values(id);

    mRSSReader->load(QUrl(listWidgetItems.last()->data(Qt::UserRole).toString()), true);

    qDeleteAll(listWidgetItems);
    qDeleteAll(widgetItems);

    mIdListWidgetItemMultiMap.remove(id);
    mIdRefreshWidgetsMultiMap.remove(id);
}

void RSSWidget::itemPressed(QListWidgetItem* listWidgetItem) {
    if (!mDoubleClicked) {
        QTimer::singleShot(300, this, SLOT(itemClickedTimeout()));
        mSingleClickedItem = listWidgetItem;
    }
}

void RSSWidget::itemClickedTimeout() {
    if (!mDoubleClicked) {
        emit rssListItemPressed(mSingleClickedItem->text());
    } else mDoubleClicked = false;
}

void RSSWidget::itemDoubleClicked(QListWidgetItem* listWidgetItem) {
    mDoubleClicked = true;

    QDesktopServices::openUrl(QUrl(listWidgetItem->data(Qt::UserRole).toString()));
}

QListWidgetItem* RSSWidget::getListWidgetItem(QString title, QString description, QUrl url) {
    QListWidgetItem* listWidgetItem = new QListWidgetItem(QString("%1\n%2").arg(title, description));
    listWidgetItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    listWidgetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    listWidgetItem->setData(Qt::UserRole, url);

    return listWidgetItem;
}

void RSSWidget::addListWidgetItem(QString key, QString title, QString description, QUrl url, QUrl imageUrl) {
    QListWidgetItem* listWidgetItem = this->getListWidgetItem(title, description, url);
    mRSSListWidget->addItem(listWidgetItem);
    mIdListWidgetItemMultiMap.insert(key, listWidgetItem);

    if (imageUrl.isEmpty()) return;

    if (mUrlListWidgetItemHash.values(imageUrl).size() == 0) mNetworkAccessManager->get(QNetworkRequest(imageUrl)); // multiple news with same image

    mUrlListWidgetItemHash.insert(imageUrl, listWidgetItem);
}

void RSSWidget::addDefaultEntry() {
    QListWidgetItem* item = this->getListWidgetItem(tr("Default entry"), tr("If you want, you can quite easily add some RSS feeds via the settings menu or the add button above."), QUrl(""));
    item->setIcon(QIcon(":/icon.png"));
    mRSSListWidget->addItem(item);
}

void RSSWidget::addRSSUrlPushButtonClicked() {
    QUrl url = QUrl::fromUserInput(mAddRSSUrlLineEdit->text());

    if (mSettings->appendRSSUrl(url)) {
        mAddRSSUrlLineEdit->clear();
        mRSSReader->load(url, true);
    }
}
