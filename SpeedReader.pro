#-------------------------------------------------
# Copyright 2014 Vanniktech - Niklas Baudy
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

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpeedReader
TEMPLATE = app
OBJECTS_DIR = .obj
MOC_DIR     = .moc
RCC_DIR     = .rcc

VERSION = $$system(git describe --tags --abbrev=0)

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
               languages/de.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    speedreadertext.cpp \
    lib/vntformatter.cpp \
    word.cpp \
    lib/VNTRSSReader/vntrsschannel.cpp \
    lib/VNTRSSReader/vntrssitem.cpp \
    lib/VNTRSSReader/vntrssreader.cpp \
    settingswindow.cpp \
    settings.cpp\
    speedreadersegment.cpp \
    lib/VNTRSSReader/vntrsscommon.cpp

HEADERS  += mainwindow.h \
    speedreadertext.h \
    lib/vntformatter.h \
    word.h \
    lib/VNTRSSReader/vntrsschannel.h \
    lib/VNTRSSReader/vntrssitem.h \
    lib/VNTRSSReader/vntrssreader.h \
    settingswindow.h \
    settings.h\
    speedreadersegment.h \
    lib/VNTRSSReader/vntrsscommon.h

FORMS    += mainwindow.ui \
    settingswindow.ui