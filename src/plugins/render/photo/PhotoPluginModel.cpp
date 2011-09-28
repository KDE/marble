//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "PhotoPluginModel.h"

// Photo Plugin
#include "FlickrParser.h"
#include "PhotoPluginItem.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"

// Qt
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QUrl>

using namespace Marble;

const QString flickrApiKey( "620131a1b82b000c9582b94effcdc636" );

PhotoPluginModel::PhotoPluginModel( const PluginManager *pluginManager,
                                    QObject *parent )
    : AbstractDataPluginModel( "photo", pluginManager, parent )
{
}

QUrl PhotoPluginModel::generateUrl( const QString& service,
                                    const QString& method,
                                    const QHash<QString,QString>& options )
{
    QString url( "" );
    
    if( service == "flickr" )
        url += "http://www.flickr.com/services/rest/";
    else
        return QUrl();
    
    url += "?method=";
    url += method;
    url += "&format=rest";
    url += "&api_key=";
    url += flickrApiKey;
    
    QHash<QString,QString>::const_iterator it = options.constBegin();
    QHash<QString,QString>::const_iterator const end = options.constEnd();
    for (; it != end; ++it ) {
        url += '&';
        url += it.key();
        url += '=';
        url += it.value();
    }
    
    return QUrl( url );
}

void PhotoPluginModel::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                           const MarbleModel *model,
                                           qint32 number )
{
    // Flickr only supports images for earth
    if( model->planetId() != "earth" ) {
        return;
    }
    
    if( box.west() <= box.east() ) {
        QString bbox( "" );
        bbox += QString::number( box.west()  * RAD2DEG ) + ',';
        bbox += QString::number( box.south()  * RAD2DEG ) + ',';
        bbox += QString::number( box.east() * RAD2DEG ) + ',';
        bbox += QString::number( box.north() * RAD2DEG );
    
        QHash<QString,QString> options;
        options.insert( "per_page", QString::number( number ) );
        options.insert( "bbox",     bbox );
        options.insert( "sort",     "interestingness-desc" );
    
        downloadDescriptionFile( generateUrl( "flickr", "flickr.photos.search", options ) );
    }
    else {
        // Flickr api doesn't support bboxes with west > east so we have to split in two boxes
        QString bboxWest( "" );
        bboxWest += QString::number( box.west() * RAD2DEG ) + ',';
        bboxWest += QString::number( box.south()  * RAD2DEG ) + ',';
        bboxWest += QString::number( 180 ) + ',';
        bboxWest += QString::number( box.north() * RAD2DEG );
        
        QHash<QString,QString> optionsWest;
        optionsWest.insert( "per_page", QString::number( number/2 ) );
        optionsWest.insert( "bbox",     bboxWest );
        optionsWest.insert( "sort",     "interestingness-desc" );
        
        downloadDescriptionFile( generateUrl( "flickr", "flickr.photos.search", optionsWest ) );
        
        
        QString bboxEast( "" );
        bboxEast += QString::number( -180 ) + ',';
        bboxEast += QString::number( box.south()  * RAD2DEG ) + ',';
        bboxEast += QString::number( box.east() * RAD2DEG ) + ',';
        bboxEast += QString::number( box.north() * RAD2DEG );
        
        QHash<QString,QString> optionsEast;
        optionsEast.insert( "per_page", QString::number( number/2 ) );
        optionsEast.insert( "bbox",     bboxEast );
        optionsEast.insert( "sort",     "interestingness-desc" );
        
        downloadDescriptionFile( generateUrl( "flickr", "flickr.photos.search", optionsEast ) );
    }
}

void PhotoPluginModel::parseFile( const QByteArray& file )
{
    QList<PhotoPluginItem*> list;
    FlickrParser parser( &list, this );
    
    parser.read( file );
    
    QList<PhotoPluginItem*>::iterator it;
    
    for( it = list.begin(); it != list.end(); ++it ) {
        if( itemExists( (*it)->id() ) ) {
            delete (*it);
            continue;
        }
        
        // Currently all Flickr images with geotags are on earth
        (*it)->setTarget( "earth" );
        downloadItemData( (*it)->photoUrl(), "thumbnail", (*it) );
        downloadItemData( (*it)->infoUrl(),  "info",      (*it) );
    }
}

#include "PhotoPluginModel.moc"
