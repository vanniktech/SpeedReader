#ifndef RSS_H
#define RSS_H

#include <QObject>
#include <QThread>
#include <QListWidget>
#include <QSignalMapper>
#include <QPushButton>

#include "rssdata.h"
#include "rssselectiondata.h"
#include "rssitem.h"
#include "databasehelper.h"
#include "thread.h"
#include "lib/VNTRSSReader/vntrssreader.h"

class RSS : public QObject
{
    Q_OBJECT
public:
    explicit RSS(QListWidget* listWidget, QObject *parent = 0);
    ~RSS();

    void load(QUrl url);
    void loadAllRSSFeeds();
    void refreshRSSList(RSSSelectionData rssSelectionData);
    void resetRSSSelectionData();

    RSSItem* markRSSItemAsSpeedRead(int rssItemIndex);
    RSSSelectionData getRSSSelectionData();
    QList<RSSChannel> getChannels();

    int unspeedreadAllCount();
    int unspeedreadTodayCount();
    int unspeedreadYesterdayCount();

    void startRefreshRSSTimer(int ms);

signals:
    void failedToOpenDatabase();
    void rssDataChanged();
    void speedread(QString summary);
    void openInWebView(QUrl url);
    void openInDefaultWebBrowser(QUrl url);
    void failedToLoadRSSChannel(QString errorMessage);

public slots:
    void deleteRSSCache();
    void deleteAllRSSSites();
    void deleteRSSSites(QList<QString> rssSite);
    void addRSSSites(QList<QString> rssSite);

private slots:
    void listItemUnSpeedReadSpeedRead(int rssItemIndex);
    void listItemSpeedRead(int rssItemIndex);
    void listItemOpenInWebView(int rssItemIndex);
    void listItemOpenInDefaultWebBrowser(int rssItemIndex);
    void refreshRSSTimeout();
    void rssDataFinished();

private:
    RSSData mRSSData;
    Thread mRSSDataThread;
    RSSSelectionData mRSSSelectionData;
    QListWidget* mListWidget;
    VNTRSSReader*   mRSSReader;
    QList<RSSItem*> mRSSItems;
    QSignalMapper* mListItemUnSpeedReadSpeedReadSignalMapper;
    QSignalMapper* mListItemSpeedReadSignalMapper;
    QSignalMapper* mListItemOpenInWebViewSignalMapper;
    QSignalMapper* mListItemOpenInDefaultWebBrowserSignalMapper;
    QMap<int, QPushButton*> mUnSpeedReadSpeedReadLabels; // little ugly hack
    QTimer* mRefreshRSSListTimer;
    QPixmap mSpeedReadPixmap, mUnSpeedReadPixmap;

    QPixmap getPixmap(bool speedread);
    RSSItem* getRSSItem(int index);
    void updateSpeedRead(RSSItem* rssItem, int rssItemIndex);
    void clearRSSList();
    void addListWidgetItem(RSSItem* rssItem, int rssItemIndex);
    void refreshRSSList();
};

#endif // RSS_H
