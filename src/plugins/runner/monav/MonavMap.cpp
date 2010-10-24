//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MonavMap.h"
#include "MarbleDebug.h"

#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"

namespace Marble
{

void MonavMap::setDirectory( const QDir &dir )
{
    m_directory = dir;
    QFileInfo boundingBox( dir, "marble.kml" );
    if ( boundingBox.exists() ) {
        parseBoundingBox( boundingBox );
    } else {
        mDebug() << "No monav bounding box given for " << boundingBox.absoluteFilePath();
    }
}

void MonavMap::parseBoundingBox( const QFileInfo &file )
{
    GeoDataLineString points;
    QFile input( file.absoluteFilePath() );
    if ( input.open( QFile::ReadOnly ) ) {
        GeoDataParser parser( GeoData_KML );
        if ( !parser.read( &input ) ) {
            mDebug() << "Could not parse file: " << parser.errorString();
            return;
        }

        GeoDocument *doc = parser.releaseDocument();
        GeoDataDocument *document = dynamic_cast<GeoDataDocument*>( doc );
        QVector<GeoDataPlacemark*> placemarks = document->placemarkList();
        if ( placemarks.size() == 1 ) {
            GeoDataPlacemark* placemark = placemarks.first();
            m_name = placemark->name();
            m_version = placemark->extendedData().value( "version" ).value().toString();
            m_date = placemark->extendedData().value( "date" ).value().toString();
            m_transport = placemark->extendedData().value( "transport" ).value().toString();
            m_payload = placemark->extendedData().value( "payload" ).value().toString();
            GeoDataMultiGeometry* geometry = dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() );
            for ( int i = 0; geometry && i < geometry->size(); ++i ) {
                GeoDataLinearRing* poly = dynamic_cast<GeoDataLinearRing*>( geometry->child( i ) );
                if ( poly ) {
                    for ( int j = 0; j < poly->size(); ++j ) {
                        points << poly->at( j );
                    }
                    m_tiles.push_back( *poly );
                }
            }
        } else {
            mDebug() << "File " << file.absoluteFilePath() << " does not contain one placemark, but " << placemarks.size();
        }
    }
    m_boundingBox = points.latLonAltBox();
}

bool MonavMap::containsPoint( const GeoDataCoordinates &point ) const
{
    // If we do not have a bounding box at all, we err on the safe side
    if ( m_tiles.isEmpty() ) {
        return true;
    }

    // Quick check for performance reasons
    if ( !m_boundingBox.contains( point ) ) {
        return false;
    }

    // GeoDataLinearRing does a 3D check, but we only have 2D data for
    // the map bounding box. Therefore the 3D info of e.g. the GPS position
    // must be ignored.
    GeoDataCoordinates flatPosition = point;
    flatPosition.setAltitude( 0.0 );
    foreach( const GeoDataLinearRing & box, m_tiles ) {
        if ( box.contains( flatPosition ) ) {
            return true;
        }
    }

    return false;
}

qint64 MonavMap::size() const
{
    qint64 result = 0;
    foreach( const QFileInfo & file, files() ) {
        result += file.size();
    }

    return result;
}

QList<QFileInfo> MonavMap::files() const
{
    QList<QFileInfo> files;
    QStringList fileNames = QStringList() << "config" << "edges" << "names" << "paths" << "types";
    foreach( const QString & file, fileNames ) {
        files << QFileInfo( m_directory, QString( "Contraction Hierarchies_" ) + file );
    }

    fileNames = QStringList() << "config" << "grid" << "index_1" << "index_2" << "index_3";
    foreach( const QString & file, fileNames ) {
        files << QFileInfo( m_directory, QString( "GPSGrid_" ) + file );
    }

    files << QFileInfo( m_directory, "plugins.ini" );
    files << QFileInfo( m_directory, "marble.kml" );
    return files;
}

void MonavMap::remove() const
{
    foreach( const QFileInfo & file, files() ) {
        QFile ( file.absoluteFilePath() ).remove();
    }
}

bool MonavMap::areaLessThan( const MonavMap &first, const MonavMap &second )
{
    if ( !first.m_tiles.isEmpty() && second.m_tiles.isEmpty() ) {
        return true;
    }

    if ( first.m_tiles.isEmpty() && !second.m_tiles.isEmpty() ) {
        return false;
    }

    qreal const areaOne = first.m_boundingBox.width() * first.m_boundingBox.height();
    qreal const areaTwo = second.m_boundingBox.width() * second.m_boundingBox.height();
    return areaOne < areaTwo;
}

bool MonavMap::nameLessThan( const MonavMap &first, const MonavMap &second )
{
    return first.name() < second.name();
}

QDir MonavMap::directory() const
{
    return m_directory;
}

QString MonavMap::transport() const
{
    return m_transport;
}

QString MonavMap::name() const
{
    return m_name;
}

QString MonavMap::version() const
{
    return m_version;
}

QString MonavMap::date() const
{
    return m_date;
}

QString MonavMap::payload() const
{
    return m_payload;
}

}
