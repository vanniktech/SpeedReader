#include "navigationdrawer.h"

#include <QPushButton>

#include "row.h"

NavigationDrawer::NavigationDrawer(QWidget *parent) : QVBoxLayout(parent) {
    this->setContentsMargins(0, 0, 0, 0);
    this->setAlignment(Qt::AlignTop);
    this->setSpacing(0);

    mIsRefreshing = false;
    mRefreshMovie = new QMovie(this);
    mRefreshMovie->setFileName(":/images/refresh.gif");
    QObject::connect(mRefreshMovie, SIGNAL(frameChanged(int)), this, SLOT(refreshMovieFrameChanged(int)));
}

NavigationDrawer::~NavigationDrawer() {

}

void NavigationDrawer::update(QList<RSSChannel> rssChannels, int allUnspeedreadCount, int unspeedreadTodayCount, int unspeedreadYesterdayCount) {
    this->remove(this);

    QWidget* widget = new QWidget();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(0);

    Row* speedread = new Row(tr("SpeedRead"), true);
    layout->addWidget(speedread);
    QObject::connect(speedread, SIGNAL(clicked()), this, SLOT(speedreadClicked()));

    QHBoxLayout* rssFeedsLayout = new QHBoxLayout();
    Row* rss = new Row(tr("RSS feeds"));
    rssFeedsLayout->addWidget(rss, 1);

    mRefreshButton = new QPushButton();
    QObject::connect(mRefreshButton, SIGNAL(clicked()), this, SIGNAL(onRefreshRSSSiteButtonClicked()));
    this->updateRefreshButton();
    rssFeedsLayout->addWidget(mRefreshButton, 0, Qt::AlignRight);

    layout->addLayout(rssFeedsLayout);

    Row* all = new Row(tr("All"), allUnspeedreadCount);
    layout->addWidget(all);
    QObject::connect(all, SIGNAL(clicked()), this, SLOT(allClicked()));

    Row* unspeedread = new Row(tr("Un-SpeedRead"), allUnspeedreadCount);
    layout->addWidget(unspeedread);
    QObject::connect(unspeedread, SIGNAL(clicked()), this, SLOT(unspeedreadClicked()));

    Row* today = new Row(tr("Today"), unspeedreadTodayCount);
    layout->addWidget(today);
    QObject::connect(today, SIGNAL(clicked()), this, SLOT(todayClicked()));

    Row* yesterday = new Row(tr("Yesterday"), unspeedreadYesterdayCount);
    layout->addWidget(yesterday);
    QObject::connect(yesterday, SIGNAL(clicked()), this, SLOT(yesterdayClicked()));

    QHBoxLayout* rssSitesLayout = new QHBoxLayout();
    Row* rssSites = new Row(tr("RSS sites"));
    rssSitesLayout->addWidget(rssSites, 1);

    QPushButton* addButton = new QPushButton();
    QObject::connect(addButton, SIGNAL(clicked()), this, SIGNAL(onAddRSSSiteButtonClicked()));
    addButton->setIcon(QIcon(":/images/add.png"));
    addButton->setToolTip(tr("Shortcut: A"));
    rssSitesLayout->addWidget(addButton, 0, Qt::AlignRight);

    layout->addLayout(rssSitesLayout);

    mRSSSitesSignalMapper = new QSignalMapper(this);

    foreach (RSSChannel rssChannel, rssChannels) {
        Row* row = new Row(rssChannel.title, rssChannel.unspeedread, rssChannel.link);
        layout->addWidget(row);
        QObject::connect(row, SIGNAL(clicked()), mRSSSitesSignalMapper, SLOT(map()));
        mRSSSitesSignalMapper->setMapping(row, rssChannel.rssSite);
    }

    QObject::connect(mRSSSitesSignalMapper, SIGNAL(mapped(QString)), this, SIGNAL(rssSiteClicked(QString)));

    widget->setLayout(layout);
    this->addWidget(widget);
}

void NavigationDrawer::speedreadClicked() {
    emit onSpeedreadClicked();
}
void NavigationDrawer::allClicked() {
    emit onAllClicked();
}
void NavigationDrawer::unspeedreadClicked() {
    emit onUnspeedreadClicked();
}
void NavigationDrawer::todayClicked() {
    emit onTodayClicked();
}
void NavigationDrawer::yesterdayClicked() {
    emit onYesterdayClicked();
}

void NavigationDrawer::refreshMovieFrameChanged(int frame) {
    Q_UNUSED(frame);

    mRefreshButton->setIcon(QIcon(mRefreshMovie->currentPixmap()));
}

void NavigationDrawer::startRefreshAnimation() {
    if (!mIsRefreshing) {
        mIsRefreshing = true;

        mRefreshMovie->start();

        this->updateRefreshButton();
    }
}

void NavigationDrawer::stopRefreshAnimation() {
    if (mIsRefreshing) {
        mIsRefreshing = false;

        mRefreshMovie->stop();

        this->updateRefreshButton();
    }
}

bool NavigationDrawer::isRefreshing() {
    return mIsRefreshing;
}

void NavigationDrawer::updateRefreshButton() {
    mRefreshButton->setEnabled(!mIsRefreshing);

    if (mIsRefreshing) {
        mRefreshButton->setIcon(QIcon());
        mRefreshButton->setToolTip("");
    } else {
        mRefreshButton->setIcon(QIcon(":/images/refresh.png"));
        mRefreshButton->setToolTip(tr("Shortcut: R"));
    }
}

void NavigationDrawer::remove(QLayout* layout) {
    while (layout->count() != 0) {
        QLayoutItem* child = layout->takeAt(0);

        if (child->layout() != 0) remove(child->layout());
        else if (child->widget() != 0) delete child->widget();

        delete child;
    }
}
