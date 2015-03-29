#-------------------------------------------------
# Copyright 2014-2015 Vanniktech - Niklas Baudy
#
# This file is part of SpeedReader.
#
# SpeedReader is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# SpeedReader is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SpeedReader. If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += webkit webkitwidgets
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpeedReader
TEMPLATE = app
OBJECTS_DIR = .obj
MOC_DIR     = .moc
RCC_DIR     = .rcc

VERSION = v1.4.0

DEFINES += APPLICATION_VERSION=\\\"$${VERSION}\\\"
DEFINES += APPLICATION_NAME=\\\"SpeedReader\\\"
DEFINES += ORGANIZATION_NAME=\\\"Vanniktech\\\"
DEFINES += ORGANIZATION_DOMAIN=\\\"http://www.vanniktech.de/\\\"

# Icons
win32{
    RC_FILE = SpeedReader.rc
}

mac {
    ICON = icon.icns
}

RESOURCES += resources.qrc

TRANSLATIONS = languages/en.ts \
               languages/de.ts \
               languages/cs.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    speedreadertext.cpp \
    lib/VNTRSSReader/vntrsschannel.cpp \
    lib/VNTRSSReader/vntrsscommon.cpp \
    lib/VNTRSSReader/vntrssitem.cpp \
    lib/VNTRSSReader/vntrssreader.cpp \
    lib/VNTRSSReader/vntprotocolhandler.cpp \
    lib/VNTRSSReader/vntatomhandler.cpp \
    lib/VNTRSSReader/vntrsshandler.cpp \
    settingswindow.cpp \
    settings.cpp\
    speedreadersegment.cpp \
    rsswebviewdialog.cpp \
    rssitem.cpp \
    databasehelper.cpp \
    row.cpp \
    checkforupdates.cpp \
    rss.cpp \
    rssdata.cpp \
    thread.cpp \
    navigationdrawer.cpp \
    i18n.cpp

HEADERS  += mainwindow.h \
    speedreadertext.h \
    lib/vntformatter.h \
    lib/VNTRSSReader/vntrsschannel.h \
    lib/VNTRSSReader/vntrsscommon.h \
    lib/VNTRSSReader/vntrssitem.h \
    lib/VNTRSSReader/vntrssreader.h \
    lib/VNTRSSReader/vntprotocolhandler.h \
    lib/VNTRSSReader/vntatomhandler.h \
    lib/VNTRSSReader/vntrsshandler.h \
    lib/VNTRSSReader/vntprotocolspecialcase.h \
    settingswindow.h \
    settings.h\
    speedreadersegment.h \
    rsswebviewdialog.h \
    rssitem.h \
    databasehelper.h \
    row.h \
    checkforupdates.h \
    rss.h \
    rssdata.h \
    rssselectiondata.h \
    thread.h \
    navigationdrawer.h \
    rsschannel.h \
    i18n.h \
    initializableqmap.h \
    speedreadertextdatasource.h

FORMS    += mainwindow.ui \
    settingswindow.ui \
    rsswebviewdialog.ui

CONFIG += c++11
