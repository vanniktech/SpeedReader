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

#include "databasehelper.h"

#ifdef Q_OS_LINUX
#include <QDir>
#endif

#ifdef Q_OS_MAC
#include <QApplication>
#include <QDir>
#endif

#include <QSqlQuery>
#include <QSqlDriver>

const QString DATABASE_NAME = "com.vanniktech.speedreader.db";

DatabaseHelper::DatabaseHelper(QObject *parent) : QObject(parent) {
    mDB = QSqlDatabase::addDatabase("QSQLITE");

    #ifdef Q_OS_MAC
        QString databaseName = QApplication::applicationDirPath().append(QDir::separator()).append(DATABASE_NAME);
        mDB.setDatabaseName(QDir::toNativeSeparators(databaseName));  // NOTE: File exists in the application private folder, in Symbian Qt implementation
    #endif

    #ifdef Q_OS_WIN32
        mDB.setDatabaseName(DATABASE_NAME);
    #endif

    #ifdef Q_OS_LINUX
        QString path(QDir::home().path());
        path.append(QDir::separator()).append(DATABASE_NAME);
        mDB.setDatabaseName(QDir::toNativeSeparators(path));
    #endif

    if (!mDB.open()) emit failedToOpen();
    else this->createTables();
}

DatabaseHelper::~DatabaseHelper() {
}

void DatabaseHelper::dropTables() {
    QSqlQuery query;
    query.exec("drop TABLE rss_channel");
    query.exec("drop TABLE rss_item");
}

void DatabaseHelper::createTables() {
    QSqlQuery query;
    query.exec("create table if not exists rss_channel ( "
        "rss_channel_id varchar(38) primary key, "
        "title TEXT, "
        "description TEXT, "
        "link varchar(512), "
        "rss_site varchar(512) UNIQUE, "
        "image_local varchar(512), "
        "image_internet varchar(512), "
        "date INTEGER"
   ")");

    query.exec("create table if not exists rss_item ("
        "rss_item_id varchar(38) primary key, "
        "rss_channel_id varchar(38), "
        "title TEXT, "
        "description TEXT, "
        "link varchar(512), "
        "date INTEGER,"
        "image_local varchar(512), "
        "image_internet varchar(512), "
        "speedread BOOLEAN"
   ")");
}

bool DatabaseHelper::beginTransaction() {
    return mDB.driver()->beginTransaction();
}
bool DatabaseHelper::commitTransaction() {
    return mDB.driver()->commitTransaction();
}

bool DatabaseHelper::update(QString tableName, QMap<QString, QVariant> contentValues, QString whereColumn, QVariant whereValue) {
    if (contentValues.size() == 0) return true;

    QString prepare("UPDATE " + tableName + " SET ");
    QMap<QString, QVariant>::iterator iColumns;
    for (iColumns = contentValues.begin(); iColumns != contentValues.end(); ++iColumns)
        prepare.append(iColumns.key() + " = ?, ");

    prepare.chop(2);
    prepare.append(" WHERE " + whereColumn + " = ?");

    QSqlQuery queryUpdate;
    queryUpdate.prepare(prepare);

    QMap<QString, QVariant>::iterator iValues;
    for (iValues = contentValues.begin(); iValues != contentValues.end(); ++iValues)
        queryUpdate.addBindValue(iValues.value());

    queryUpdate.addBindValue(whereValue);
    return queryUpdate.exec();
}

bool DatabaseHelper::update(QString tableName, QString column, QVariant value, QString whereColumn, QVariant whereValue) {
    QSqlQuery queryUpdate;
    queryUpdate.prepare("UPDATE " + tableName + " SET " + column + " = ? WHERE " + whereColumn + " = ?");
    queryUpdate.addBindValue(value);
    queryUpdate.addBindValue(whereValue);
    return queryUpdate.exec();
}

bool DatabaseHelper::insert(QString tableName, QList<QVariant> variants) {
    QString values = QString("?,").repeated(variants.size());
    values = values.remove(values.length() - 1, 1);

    QSqlQuery queryInsert;
    queryInsert.prepare("INSERT INTO " + tableName + " VALUES (" + values + ")");

    foreach (QVariant variant, variants) queryInsert.addBindValue(variant);

    return queryInsert.exec();
}

int DatabaseHelper::getSingleInt(QString sqlQuery, QList<QVariant> variants, int defaultValue) {
    QSqlQuery query;
    query.prepare(sqlQuery);
    foreach (QVariant variant, variants) query.addBindValue(variant);
    query.exec();

    int value = defaultValue;

    if (query.next()) value = query.value(0).value<int>();

    return value;
}
