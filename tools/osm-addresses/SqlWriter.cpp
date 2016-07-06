//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "SqlWriter.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Marble
{

SqlWriter::SqlWriter( const QString &filename, QObject* parent ) :
    Writer( parent ), m_placemarkId( 0 )
{
    QSqlDatabase database = QSqlDatabase::addDatabase( "QSQLITE" );
    database.setDatabaseName( filename );
    if ( !database.open() ) {
        qCritical() << "Failed to connect to database";
        return;
    }

    execQuery( "DROP TABLE IF EXISTS placemarks;" );
    execQuery( "CREATE TABLE placemarks ("
               " regionId INTEGER,"
               " nameId INTEGER,"
               " number VARCHAR(8),"
               " category INTEGER,"
               " lon FLOAT(8),"
               " lat FLOAT(8) )" );
    execQuery( "DROP TABLE IF EXISTS names" );
    execQuery( "CREATE TABLE names ("
               " id INTEGER PRIMARY KEY,"
               " name VARCHAR(50) )" );
    execQuery( "DROP TABLE IF EXISTS regions" );
    execQuery( "CREATE TABLE regions ("
               " id INTEGER PRIMARY KEY,"
               " parent INTEGER NOT NULL,"
               " lft INTEGER NOT NULL,"
               " rgt INTEGER NOT NULL,"
               " name VARCHAR(50),"
               " lon FLOAT(8),"
               " lat FLOAT(8) )" );
    execQuery( "DROP VIEW IF EXISTS places" );
    execQuery( "CREATE VIEW places AS "
               " SELECT"
               "  placemarks.regionId AS region,"
               "  names.name AS name,"
               "  placemarks.number AS number,"
               "  placemarks.category AS category,"
               "  placemarks.lon AS lon,"
               "  placemarks.lat AS lat"
               " FROM names"
               " INNER JOIN placemarks"
               " ON names.id=placemarks.nameId" );
    execQuery( "BEGIN TRANSACTION" );
}

SqlWriter::~SqlWriter()
{
    execQuery( "END TRANSACTION" );
    execQuery( "CREATE INDEX namesIndex ON names(name)" );
    execQuery( "CREATE INDEX placemarksIndex ON placemarks(regionId,nameId,category)" );
    execQuery( "CREATE INDEX regionsIndex ON regions(name,parent,lft,rgt)" );
}

void SqlWriter::addOsmRegion( const OsmRegion &region )
{
    QSqlQuery query;
    query.prepare( "INSERT INTO regions"
                   " (id, parent, lft, rgt, name, lon, lat)"
                   " VALUES (?, ?, ?, ?, ?, ?, ?)" );
    query.addBindValue( ( qint32 ) region.identifier() );
    query.addBindValue( ( qint32 ) region.parentIdentifier() );
    query.addBindValue( ( qint32 ) region.left() );
    query.addBindValue( ( qint32 ) region.right() );
    query.addBindValue( region.name() );
    query.addBindValue( region.longitude() );
    query.addBindValue( region.latitude() );
    execQuery( query );
}

void SqlWriter::addOsmPlacemark( const OsmPlacemark &placemark )
{
    if ( m_lastPlacemark.second != placemark.name() && !m_placemarks.contains( placemark.name() ) ) {
        m_lastPlacemark.first = ++m_placemarkId;
        m_lastPlacemark.second = placemark.name();
        m_placemarks[m_lastPlacemark.second] = m_lastPlacemark.first;

        QSqlQuery insertQuery;
        insertQuery.prepare( "INSERT INTO names"
                             " (id, name)"
                             " VALUES (?, ?)" );
        insertQuery.addBindValue( m_lastPlacemark.first );
        insertQuery.addBindValue( m_lastPlacemark.second );
        execQuery( insertQuery );
    }

    Q_ASSERT( m_placemarks.contains( placemark.name() ) );

    QSqlQuery query;
    query.prepare( "INSERT INTO placemarks"
                   " (regionId, nameId, number, category, lon, lat)"
                   " VALUES (?, ?, ?, ?, ?, ?)" );
    query.addBindValue( ( qint32 ) placemark.regionId() );
    query.addBindValue( m_placemarks[placemark.name()] );
    query.addBindValue( placemark.houseNumber() );
    query.addBindValue( ( qint32 ) placemark.category() );
    query.addBindValue( placemark.longitude() );
    query.addBindValue( placemark.latitude() );
    execQuery( query );
}

void SqlWriter::execQuery( const QString &query ) const
{
    QSqlQuery sqlQuery( query );
    if ( sqlQuery.lastError().isValid() ) {
        qCritical() << "Problems occurred when executing the query" << query;
        qCritical() << "SQL error: " << sqlQuery.lastError();
    }
}

void SqlWriter::execQuery( QSqlQuery &query ) const
{
    query.exec();
    if ( query.lastError().isValid() ) {
        qCritical() << "Problems occurred when executing the query" << query.executedQuery();
        qCritical() << "SQL error: " << query.lastError();
    }
}

}
