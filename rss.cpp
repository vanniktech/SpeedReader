#include "rss.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QtSvg/QSvgRenderer>
#include <QPainter>
#include <QLabel>

#include "settings.h"

const int MAX_RSS_DESCRIPTION_LENGTH = 300;

RSS::RSS(QListWidget *listWidget, QObject *parent) : QObject(parent), mRSSDataThread(this) {
    mListWidget = listWidget;

    mSpeedReadPixmap = QPixmap(":/images/dot_green.png");
    mUnSpeedReadPixmap = QPixmap(":/images/dot_red.png");

    mRSSData.moveToThread(&mRSSDataThread);
    QObject::connect(&mRSSData, SIGNAL(finished()), this, SLOT(rssDataFinished()));
    QObject::connect(&mRSSData, SIGNAL(failedToLoadRSSChannel(QString)), this, SIGNAL(failedToLoadRSSChannel(QString)));
    QObject::connect(&mRSSData, SIGNAL(failedToOpenDatabase()), this, SIGNAL(failedToOpenDatabase()));
    mRSSDataThread.start();

    mRSSReader = new VNTRSSReader(this);
    QObject::connect(mRSSReader, SIGNAL(loadedRSS(QList<VNTRSSChannel*>)), &mRSSData, SLOT(loadedRSS(QList<VNTRSSChannel*>)));

    mRefreshRSSListTimer = new QTimer(this);
    QObject::connect(mRefreshRSSListTimer, SIGNAL(timeout()), this, SLOT(refreshRSSTimeout()));
}

RSS::~RSS() {
    this->clearRSSList();
}

void RSS::startRefreshRSSTimer(int ms) {
    mRefreshRSSListTimer->stop();

    if (ms > 0) mRefreshRSSListTimer->start(ms);
}

void RSS::refreshRSSTimeout() {
    this->loadAllRSSFeeds();
}

void RSS::rssDataFinished() {
    emit rssDataChanged();

    this->refreshRSSList();
}

void RSS::clearRSSList() {
    qDeleteAll(mRSSItems.begin(), mRSSItems.end());
    mRSSItems.clear();
}

void RSS::refreshRSSList(RSSSelectionData rssSelectionData) {
    mRSSSelectionData = rssSelectionData;
    this->refreshRSSList();
}

void RSS::resetRSSSelectionData() {
    mRSSSelectionData.selection = NONE;
}

void RSS::refreshRSSList() {
    if (mRSSSelectionData.selection != NONE) {
        mListWidget->clear();

        this->clearRSSList();
        mUnSpeedReadSpeedReadLabels.clear();

        mListItemUnSpeedReadSpeedReadSignalMapper = new QSignalMapper(this);
        mListItemSpeedReadSignalMapper = new QSignalMapper(this);
        mListItemOpenInWebViewSignalMapper = new QSignalMapper(this);
        mListItemOpenInDefaultWebBrowserSignalMapper = new QSignalMapper(this);

        mRSSItems = mRSSData.getRSSItems(mRSSSelectionData);

        for (int i = 0; i < mRSSItems.count(); i++) {
            this->addListWidgetItem(mRSSItems.at(i), i);
        }

        QObject::connect(mListItemUnSpeedReadSpeedReadSignalMapper, SIGNAL(mapped(int)), this, SLOT(listItemUnSpeedReadSpeedRead(int)));
        QObject::connect(mListItemSpeedReadSignalMapper, SIGNAL(mapped(int)), this, SLOT(listItemSpeedRead(int)));
        QObject::connect(mListItemOpenInWebViewSignalMapper, SIGNAL(mapped(int)), this, SLOT(listItemOpenInWebView(int)));
        QObject::connect(mListItemOpenInDefaultWebBrowserSignalMapper, SIGNAL(mapped(int)), this, SLOT(listItemOpenInDefaultWebBrowser(int)));
    }
}

void RSS::addListWidgetItem(RSSItem* rssItem, int rssItemIndex) {
    QString description = rssItem->getDescription();
    if (description.length() > MAX_RSS_DESCRIPTION_LENGTH) description = description.mid(0, MAX_RSS_DESCRIPTION_LENGTH).append(" ...");

    QListWidgetItem* listWidgetItem = new QListWidgetItem();
    listWidgetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    listWidgetItem->setSizeHint(QSize(mListWidget->width(), 200));
    mListWidget->addItem(listWidgetItem);

    QWidget* widget = new QWidget();

    QVBoxLayout* rootLayout = new QVBoxLayout();
    rootLayout->setAlignment(Qt::AlignTop);

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setAlignment(Qt::AlignLeft);

    if (!rssItem->getImageLocal().isEmpty()) {
        QLabel* imageLabel = new QLabel();
        QPixmap pixmap = QPixmap::fromImage(rssItem->getImage());
        imageLabel->setPixmap(pixmap);

        contentLayout->addWidget(imageLabel, 0, Qt::AlignTop);
    }

    QVBoxLayout* informationLayout = new QVBoxLayout();
    informationLayout->setAlignment(Qt::AlignTop);

    QLabel* titleLabel = new QLabel("<b>" + rssItem->getTitle()  + "</b>");
    titleLabel->setWordWrap(true);
    informationLayout->addWidget(titleLabel);

    QLabel* descriptionLabel = new QLabel(description);
    descriptionLabel->setWordWrap(true);
    informationLayout->addWidget(descriptionLabel);

    QLabel* dateLabel = new QLabel(rssItem->getDate().toString());
    informationLayout->addWidget(dateLabel);

    contentLayout->addLayout(informationLayout);

    QPushButton* unspeedreadSpeedReadPushButton = new QPushButton();
    unspeedreadSpeedReadPushButton->setFlat(true);
    unspeedreadSpeedReadPushButton->setFocusPolicy(Qt::NoFocus);
    unspeedreadSpeedReadPushButton->setStyleSheet("QPushButton { border: none; margin: 0px; padding: 0px; }");

    QPixmap iconPixmap = this->getPixmap(rssItem->isSpeedRead());
    unspeedreadSpeedReadPushButton->setIcon(QIcon(iconPixmap));
    unspeedreadSpeedReadPushButton->setIconSize(iconPixmap.rect().size());

    mUnSpeedReadSpeedReadLabels.insert(rssItemIndex, unspeedreadSpeedReadPushButton);
    QObject::connect(unspeedreadSpeedReadPushButton, SIGNAL(clicked()), mListItemUnSpeedReadSpeedReadSignalMapper, SLOT(map()));
    mListItemUnSpeedReadSpeedReadSignalMapper->setMapping(unspeedreadSpeedReadPushButton, rssItemIndex);

    contentLayout->addWidget(unspeedreadSpeedReadPushButton, 0, Qt::AlignTop | Qt::AlignRight);

    rootLayout->addLayout(contentLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignLeft);

    QPushButton* speedreadButton = new QPushButton(tr("SpeedRead"));
    QObject::connect(speedreadButton, SIGNAL(clicked()), mListItemSpeedReadSignalMapper, SLOT(map()));
    mListItemSpeedReadSignalMapper->setMapping(speedreadButton, rssItemIndex);
    buttonLayout->addWidget(speedreadButton);

    QPushButton* openInWebViewButton = new QPushButton(tr("Open in web view"));
    QObject::connect(openInWebViewButton, SIGNAL(clicked()), mListItemOpenInWebViewSignalMapper, SLOT(map()));
    mListItemOpenInWebViewSignalMapper->setMapping(openInWebViewButton, rssItemIndex);
    buttonLayout->addWidget(openInWebViewButton);

    QPushButton* openInDefaultWebBrowser = new QPushButton(tr("Open in default web browser"));
    QObject::connect(openInDefaultWebBrowser, SIGNAL(clicked()), mListItemOpenInDefaultWebBrowserSignalMapper, SLOT(map()));
    mListItemOpenInDefaultWebBrowserSignalMapper->setMapping(openInDefaultWebBrowser, rssItemIndex);
    buttonLayout->addWidget(openInDefaultWebBrowser);

    rootLayout->addLayout(buttonLayout);

    widget->setLayout(rootLayout);

    listWidgetItem->setSizeHint(widget->sizeHint());
    mListWidget->setItemWidget(listWidgetItem, widget);
}

void RSS::load(QUrl url) {
    mRSSReader->load(url);
}

void RSS::loadAllRSSFeeds() {
    QList<QUrl> rssSites = Settings::getInstance()->getRSSSites();

    if (rssSites.size() > 0) {
        mRSSReader->load(rssSites);
    } else {
        emit rssDataChanged();
    }
}

RSSItem* RSS::markRSSItemAsSpeedRead(int rssItemIndex) {
    RSSItem* rssItem = this->getRSSItem(rssItemIndex);

    if (rssItem != NULL && !rssItem->isSpeedRead()) {
        rssItem->setSpeedRead(true);
        this->updateSpeedRead(rssItem, rssItemIndex);
    }

    return rssItem;
}

void RSS::updateSpeedRead(RSSItem* rssItem, int rssItemIndex) {
    if (mRSSData.setSpeedRead(rssItem->getRSSItemId(), rssItem->isSpeedRead())) {
        QPushButton* pushButton = mUnSpeedReadSpeedReadLabels.value(rssItemIndex);
        QPixmap iconPixmap = this->getPixmap(rssItem->isSpeedRead());
        pushButton->setIcon(QIcon(iconPixmap));
        pushButton->setIconSize(iconPixmap.rect().size());

        emit rssDataChanged();
    }
}

RSSSelectionData RSS::getRSSSelectionData() {
    return mRSSSelectionData;
}

QList<RSSChannel> RSS::getChannels() {
    return mRSSData.getChannels();
}

void RSS::deleteRSSCache() {
    mRSSData.deleteData();

    this->loadAllRSSFeeds();
}

void RSS::deleteAllRSSSites() {
    mRSSData.deleteData();

    emit rssDataChanged();
}

void RSS::deleteRSSSites(QList<QString> rssSites) {
    if (rssSites.size() > 0) {
        mRSSData.deleteRSSSites(rssSites);
        emit rssDataChanged();
    }
}

void RSS::addRSSSites(QList<QString> rssSites) {
    QList<QUrl> rssSitesUrl;

    foreach (QString rssSite, rssSites) {
        QUrl url = QUrl::fromUserInput(rssSite);

        if (url.isValid()) rssSitesUrl.append(url);
    }

    mRSSReader->load(rssSitesUrl);
}

void RSS::listItemUnSpeedReadSpeedRead(int rssItemIndex) {
    RSSItem* rssItem = this->getRSSItem(rssItemIndex);

    if (rssItem != NULL) {
        rssItem->flipSpeedRead();
        this->updateSpeedRead(rssItem, rssItemIndex);
    }
}

void RSS::listItemSpeedRead(int rssItemIndex) {
    RSSItem* rssItem = this->markRSSItemAsSpeedRead(rssItemIndex);

    if (rssItem != NULL) emit speedread(rssItem->getTitle() + "\n" + rssItem->getDescription());
}

void RSS::listItemOpenInDefaultWebBrowser(int rssItemIndex) {
    RSSItem* rssItem = this->markRSSItemAsSpeedRead(rssItemIndex);

    if (rssItem != NULL) emit openInDefaultWebBrowser(rssItem->getLink());
}

void RSS::listItemOpenInWebView(int rssItemIndex) {
    RSSItem* rssItem = this->markRSSItemAsSpeedRead(rssItemIndex);

    if (rssItem != NULL) emit openInWebView(rssItem->getLink());
}

int RSS::unspeedreadAllCount() {
    return mRSSData.unspeedreadAllCount();
}

int RSS::unspeedreadTodayCount() {
    return mRSSData.unspeedreadTodayCount();
}

int RSS::unspeedreadYesterdayCount() {
    return mRSSData.unspeedreadYesterdayCount();
}

QPixmap RSS::getPixmap(bool speedread) {
    return speedread ? mSpeedReadPixmap : mUnSpeedReadPixmap;
}

RSSItem* RSS::getRSSItem(int index) {
    return index < mRSSItems.size() ? mRSSItems.at(index) : NULL;
}
