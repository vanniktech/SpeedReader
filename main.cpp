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

#include "mainwindow.h"
#include <QApplication>

#include <QFile>

int main(int argc, char* argv[]) {
    qRegisterMetaType<SpeedReaderText::SpeedReaderStatus>("SpeedReaderText::SpeedReaderStatus");

    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);

    QApplication a(argc, argv);
    MainWindow w;
    a.setWindowIcon(QIcon(":/icon.png"));

    // Command line
    QStringList args = QCoreApplication::arguments();
    int argsLength = args.length();
    bool r = false, t = false;

    for (int i = 0; i < argsLength; i++) {
        QString string = args[i];
        bool anotherArg = i + 1 <= argsLength;

        if (string == "-t" && anotherArg && t == false && (t = true)) {
            w.setReadWidgetText(args[i + 1]);
        } else if (string == "-f" && anotherArg && t == false && (t = true)) {
            QFile file(args[i + 1]);
            if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << QString("Could not find file %1").arg(args[i + 1]);
                continue;
            }

            QString data(file.readAll());
            w.setReadWidgetText(data);
        } else if (string == "-r" && r == false && (r = true)) w.pasteWidgetReadButtonClicked();
        else if (string == "-g" && r == false && (r = true)) w.pasteWidgetGoButtonClicked();
        else if (string == "-v") {
            qDebug() << "Version" << APPLICATION_VERSION;
            return 0;
        }
        else if (string == "-h") {
            qDebug() << QString("SpeedReader ( [-f <file> | -t <text>] [-r | -g] ) | ( [-v] | [-h] )");
            qDebug() << "-f <file> 	will open every file with content type text/plain and paste the content into the textarea";
            qDebug() << "-t <text> 	will automatically paste the given text into the textarea";
            qDebug() << "-r 		this actually does the same as pressing the read button";
            qDebug() << "-g 		this will perform the click on the go button";
            qDebug() << "-v 		prints out the version";
            qDebug() << "-h 		will show you all commands, which are supported with your version";
            return 0;
        }
    }

    // w.setMaximumWidth(940); // Release
    w.show();

    return a.exec();
}
