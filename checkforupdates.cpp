#include "checkforupdates.h"

#include <QUrl>
#include <QNetworkReply>
#include <QMessageBox>
#include <QDesktopServices>

#include "settings.h"

CheckForUpdates::CheckForUpdates(QString urlTxt, QString urlSite, QObject *parent) : QObject(parent) {
    mNetworkAccessManagerUpdate = new QNetworkAccessManager(this);
    QObject::connect(mNetworkAccessManagerUpdate, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateReplyFinished(QNetworkReply*)));

    mUrlTxt = urlTxt;
    mUrlSite = urlSite;
}

void CheckForUpdates::checkForUpdates(bool silent) {
    mSilent = silent;

    if (mSilent && !Settings::getInstance()->autoUpdate()) {
        return;
    }

    mNetworkAccessManagerUpdate->get(QNetworkRequest(QUrl(mUrlTxt)));
}

void CheckForUpdates::updateReplyFinished(QNetworkReply* networkReply) {
    QString value = QString(networkReply->readAll());
    networkReply->deleteLater();

    if (!mSilent && value.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText(tr("Could not get information about latest version"));
        msgBox.exec();

        return;
    }

    if (!mSilent && value == APPLICATION_VERSION) {
        QMessageBox msgBox;
        msgBox.setText(tr("Your version is up to date"));
        msgBox.exec();
    } else if (value > APPLICATION_VERSION) {
        if (mSilent) {
            QMessageBox messageBox(tr("Check for updates"), tr("New version %1 is available.").arg(value), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Escape, QMessageBox::No, QMessageBox::Cancel | QMessageBox::Default);
            messageBox.setButtonText(QMessageBox::Yes, tr("Never remind me again"));
            messageBox.setButtonText(QMessageBox::No, tr("Remind me tomorrow"));
            messageBox.setButtonText(QMessageBox::Cancel, tr("Download"));

            const int response = messageBox.exec();

            if (response == QMessageBox::Cancel) {
                QDesktopServices::openUrl(QUrl(mUrlSite));
            } else if (response == QMessageBox::No) {
                Settings::getInstance()->setAutoUpdateTomorrow();
            } else if (response == QMessageBox::Yes) {
                Settings::getInstance()->setAutoUpdateNeverEver();
            }
        } else {
            QMessageBox messageBox(tr("Check for updates"), tr("New version %1 is available. Do you want to download it?").arg(value), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape, QMessageBox::NoButton);
            messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
            messageBox.setButtonText(QMessageBox::No, tr("No"));

            if (messageBox.exec() == QMessageBox::Yes) {
                QDesktopServices::openUrl(QUrl(mUrlSite));
            }
        }
    }
}
