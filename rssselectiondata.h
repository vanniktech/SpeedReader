#ifndef RSSSELECTIONDATA_H
#define RSSSELECTIONDATA_H

#include <QUrl>

enum RSSSelection { ALL, UNSPEEDREAD, TODAY, YESTERDAY, SITE, NONE };

struct RSSSelectionData {
    RSSSelection selection;
    QUrl rssSite;
};

#endif // RSSSELECTIONDATA_H
