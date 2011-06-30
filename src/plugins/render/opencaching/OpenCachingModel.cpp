//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingModel.h"
#include "OpenCachingItem.h"

#include "global.h"
#include "MarbleModel.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"
#include "OpenCachingCache.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtXml/QXmlStreamReader>

namespace Marble {

class OpenCachingModelPrivate
{
public:
    QHash<QString, QVariant> parseCache( QXmlStreamReader& reader );
    QHash<QString, QVariant> parseLogEntry( QXmlStreamReader& reader );
    QHash<QString, QVariant> parseDescription( QXmlStreamReader& reader );
};

QHash<QString, QVariant> OpenCachingModelPrivate::parseCache( QXmlStreamReader& reader )
{
    QHash<QString, QVariant> cache;
    while ( !reader.atEnd() ) {
        if ( reader.isStartElement() && reader.name() != "cache" ) {
            if( reader.name() == "id" ) {
                cache["id"] = reader.attributes().value("id").toString();
            }
            else if( reader.name() != "attributes" && reader.name() != "attribute" ) {
                cache[reader.name().toString()] = reader.readElementText();
            }
        }
        else if( reader.isEndElement() && reader.name() == "cache" ) {
            return cache;
        }
        reader.readNext();
    }
    return QHash<QString, QVariant>();
}

QHash<QString, QVariant> OpenCachingModelPrivate::parseLogEntry( QXmlStreamReader& reader )
{
    QHash<QString, QVariant> cacheLogEntry;
    while ( !reader.atEnd() ) {
        if ( reader.isStartElement() && reader.name() != "cachelog" ) {
            cacheLogEntry[reader.name().toString()] = reader.readElementText();
        }
        else if( reader.isEndElement() && reader.name() == "cachelog" ) {
            return cacheLogEntry;
        }
        reader.readNext();
    }
    return QHash<QString, QVariant>();
}

QHash<QString, QVariant> OpenCachingModelPrivate::parseDescription( QXmlStreamReader& reader )
{
    QHash<QString, QVariant> cacheDesc;
    while ( !reader.atEnd() ) {
        if ( reader.isStartElement() && reader.name() != "cachedesc" ) {
            cacheDesc[reader.name().toString()] = reader.readElementText();
        }
        else if( reader.isEndElement() && reader.name() == "cachedesc" ) {
            return cacheDesc;
        }
        reader.readNext();
    }
    return QHash<QString, QVariant>();
}

OpenCachingModel::OpenCachingModel( PluginManager *pluginManager, QObject *parent )
    : AbstractDataPluginModel( "opencaching", pluginManager, parent ),
      m_numResults( numberOfItemsOnScreen ),
      m_minDifficulty( 0.0 ),
      m_maxDistance( 20.0 ),
      m_startDate( QDateTime::fromString( "2006-01-01", "yyyy-MM-dd" ) ),
      m_endDate( QDateTime::currentDateTime() ),
      d( new OpenCachingModelPrivate )
{
}

OpenCachingModel::~OpenCachingModel()
{
}

void OpenCachingModel::setNumResults( int numResults )
{
    m_numResults = numResults;
}

void OpenCachingModel::setMinDifficulty( double minDifficulty )
{
    m_minDifficulty = minDifficulty;
}

void OpenCachingModel::setStartDate( const QDateTime& startDate )
{
    m_startDate = startDate;
}

void OpenCachingModel::setEndDate( const QDateTime& endDate )
{
    m_endDate = endDate;
}

void OpenCachingModel::getAdditionalItems( const GeoDataLatLonAltBox& box, const MarbleModel *model, qint32 number )
{
    Q_UNUSED( number );

    if( model->planetId() != "earth" ) {
        return;
    }

    // http://www.opencaching.de/doc/xml/xml11.htm
    QString openCachingUrl( "http://www.opencaching.de/xml/ocxml11.php" );
    openCachingUrl += "?modifiedsince=" + m_startDate.toString( "yyyyMMddhhmmss" );
    openCachingUrl += "&cache=1&cachedesc=1&picture=0&cachelog=1&removedobject=0";
    openCachingUrl += "&lat=" + QString::number( box.center().latitude() * RAD2DEG );
    openCachingUrl += "&lon=" + QString::number( box.center().longitude() * RAD2DEG );
    openCachingUrl += "&distance=" + QString::number( m_maxDistance );
    openCachingUrl += "&charset=utf-8&cdata=0&session=0&zip=0";
    downloadDescriptionFile( QUrl( openCachingUrl ) );
}

void OpenCachingModel::parseFile( const QByteArray& file )
{
    QXmlStreamReader reader( file );
    QXmlStreamReader::TokenType token;
    QHash<int, OpenCachingCache> caches;
    QHash<int, QHash<QString, OpenCachingCacheDescription> > descriptions;
    QHash<int, OpenCachingCacheLog> logs;

    mDebug() << "start parsing";
    while( !reader.atEnd() && !reader.hasError() ) {
        token = reader.readNext();
        if( token == QXmlStreamReader::StartDocument ) {
            continue;
        }
        if( token == QXmlStreamReader::StartElement ) {
            if( reader.name() == "cache" ) {
                OpenCachingCache cache = d->parseCache( reader );
                caches[cache.id()] = cache;
            }
            else if( reader.name() == "cachedesc" ) {
                OpenCachingCacheDescription description = d->parseDescription( reader );
                descriptions[description.cacheId()][description.language()] = description;
            }
            else if( reader.name() == "cachelog" ) {
                OpenCachingCacheLogEntry logEntry = d->parseLogEntry( reader );
                logs[logEntry.cacheId()].addLogEntry( logEntry );
            }
        }
    }

    mDebug() << "finished parsing, " << caches.size() << " elements:" << caches.keys();
    foreach( const int key, caches.keys() ) {
        caches[key].setDescription( descriptions[key] );
        caches[key].setLog( logs[key] );
        addItemToList( new OpenCachingItem( caches[key], this ) );
    }
}

}

#include "OpenCachingModel.moc"
