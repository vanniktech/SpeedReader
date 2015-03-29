/*
    Copyright 2014-2015 Vanniktech - Niklas Baudy

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

#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>

class DatabaseHelper : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseHelper(QObject *parent = 0);
    ~DatabaseHelper();
    void dropTables();
    void createTables();
    bool beginTransaction();
    bool commitTransaction();
    bool update(QString tableName, QMap<QString, QVariant> contentValues, QString whereColumn, QVariant whereValue);
    bool update(QString tableName, QString column, QVariant value, QString whereColumn, QVariant whereValue);
    bool insert(QString tableName, QList<QVariant> variants);
    int getSingleInt(QString sqlQuery, QList<QVariant> variants, int defaultValue);

private:
    QSqlDatabase mDB;

signals:
    void failedToOpen();

};

#endif // DATABASEHELPER_H
