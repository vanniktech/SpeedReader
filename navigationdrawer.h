#ifndef NAVIGATIONDRAWER_H
#define NAVIGATIONDRAWER_H

#include <QHBoxLayout>
#include <QSignalMapper>
#include <QMovie>
#include <QPushButton>

#include "rsschannel.h"

class NavigationDrawer : public QVBoxLayout {
    Q_OBJECT
public:
    explicit NavigationDrawer(QWidget *parent = 0);
    ~NavigationDrawer();

    void update(QList<RSSChannel> rssChannels, int allUnspeedreadCount, int unspeedreadTodayCount, int unspeedreadYesterdayCount);

    void startRefreshAnimation();
    void stopRefreshAnimation();
    bool isRefreshing();

signals:
    void onAddRSSSiteButtonClicked();
    void onRefreshRSSSiteButtonClicked();
    void onSpeedreadClicked();
    void onAllClicked();
    void onUnspeedreadClicked();
    void onTodayClicked();
    void onYesterdayClicked();
    void rssSiteClicked(QString rssSite);

public slots:

private slots:
    void speedreadClicked();
    void allClicked();
    void unspeedreadClicked();
    void todayClicked();
    void yesterdayClicked();
    void refreshMovieFrameChanged(int frame);

private:
    QSignalMapper* mRSSSitesSignalMapper;
    QMovie* mRefreshMovie;
    QPushButton* mRefreshButton;

    bool mIsRefreshing;

    void remove(QLayout* layout);

    void updateRefreshButton();
};

#endif // NAVIGATIONDRAWER_H
