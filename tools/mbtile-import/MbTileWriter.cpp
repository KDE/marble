//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "MbTileWriter.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <iostream>
#include <iomanip>

namespace Marble
{

MbTileWriter::MbTileWriter(const QString &filename, const QString &extension) :
    m_overwriteTiles(true),
    m_reportProgress(true),
    m_tileCounter(0),
    m_commitInterval(10000)
{
    bool const exists = QFileInfo(filename).exists();

    QSqlDatabase database = QSqlDatabase::addDatabase( "QSQLITE" );
    database.setDatabaseName( filename );
    if ( !database.open() ) {
        qCritical() << "Failed to connect to database";
        return;
    }

    if (!exists) {
        execQuery("PRAGMA application_id = 0x4d504258"); // MBTiles tileset, see https://www.sqlite.org/src/artifact?ci=trunk&filename=magic.txt
        execQuery("CREATE TABLE tiles (zoom_level integer, tile_column integer, tile_row integer, tile_data blob);");
        execQuery("CREATE UNIQUE INDEX tile_index ON tiles(zoom_level, tile_column, tile_row);");

        execQuery("CREATE TABLE metadata (name text, value text);");
        setMetaData("name", "Marble Vector OSM");
        setMetaData("type", "baselayer");
        setMetaData("version", "1.0");
        setMetaData("description", "A global roadmap created by the OpenStreetMap (OSM) project");
        setMetaData("format", extension);
        setMetaData("attribution", "Data from <a href=\"http://openstreetmap.org/\">OpenStreetMap</a> and <a href=\"http://www.naturalearthdata.com/\">Natural Earth</a> contributors");
    }
    execQuery("BEGIN TRANSACTION");
}

MbTileWriter::~MbTileWriter()
{
    execQuery("END TRANSACTION");
    if (m_reportProgress) {
        std::cout << std::endl;
    }
}

void MbTileWriter::setOverwriteTiles(bool overwrite)
{
    m_overwriteTiles = overwrite;
}

void MbTileWriter::setReportProgress(bool report)
{
    m_reportProgress = report;
}

void MbTileWriter::setCommitInterval(int interval)
{
    m_commitInterval = interval;
}

void MbTileWriter::addTile(const QFileInfo &file, qint32 x, qint32 y, qint32 z)
{
    if (!m_overwriteTiles && hasTile(x, y, z)) {
        if (m_reportProgress) {
            std::cout << " Skipping existing " << z << '/' << x << '/' << y << '\r';
            std::cout.flush();
        }
        return;
    }

    if (m_reportProgress && m_tileCounter % 500 == 0) {
        std::cout << "Tile " << std::right << std::setw(10) << m_tileCounter << ": ";
        std::cout << "Adding " << z << '/' << x << '/' << y << '\r';
        std::cout.flush();
    }

    QFile tileContent(file.absoluteFilePath());
    tileContent.open(QFile::ReadOnly);
    addTile(&tileContent, x, y, z);
}

void MbTileWriter::addTile(QIODevice *device, qint32 x, qint32 y, qint32 z)
{
    ++m_tileCounter;
    if (m_commitInterval > 0 && m_tileCounter % m_commitInterval == 0) {
        execQuery("END TRANSACTION");
        execQuery("BEGIN TRANSACTION");
    }

    QSqlQuery query;
    query.prepare( "INSERT OR REPLACE INTO tiles"
                   " (zoom_level, tile_column, tile_row, tile_data)"
                   " VALUES (?, ?, ?, ?)" );
    query.addBindValue(z);
    query.addBindValue(x);
    query.addBindValue(y);
    query.addBindValue(device->readAll());
    execQuery(query);
}

bool MbTileWriter::hasTile(qint32 x, qint32 y, qint32 z) const
{
    QSqlQuery query;
    query.prepare( "SELECT EXISTS(SELECT 1 FROM tiles"
                   " WHERE zoom_level=? AND tile_column=? AND tile_row=?);");
    query.addBindValue(z);
    query.addBindValue(x);
    query.addBindValue(y);
    query.exec();
    if (query.lastError().isValid()) {
        qCritical() << "Problems occurred when executing the query" << query.executedQuery();
        qCritical() << "SQL error: " << query.lastError();
    } else {
        if (query.next()) {
            return query.value(0).toBool();
        }
    }
    return false;
}

void MbTileWriter::execQuery( const QString &query ) const
{
    QSqlQuery sqlQuery( query );
    if ( sqlQuery.lastError().isValid() ) {
        qCritical() << "Problems occurred when executing the query" << query;
        qCritical() << "SQL error: " << sqlQuery.lastError();
    }
}

void MbTileWriter::execQuery( QSqlQuery &query ) const
{
    query.exec();
    if ( query.lastError().isValid() ) {
        qCritical() << "Problems occurred when executing the query" << query.executedQuery();
        qCritical() << "SQL error: " << query.lastError();
    }
}

void MbTileWriter::setMetaData(const QString &name, const QString &value)
{
    QSqlQuery query;
    query.prepare("INSERT INTO metadata (name, value) VALUES (?, ?)");
    query.addBindValue(name);
    query.addBindValue(value);
    execQuery(query);
}

}
