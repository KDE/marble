//
// This file is part of the Marble Desktop Globe.
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
#include "MarbleDataFacade.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QUrl>

using namespace Marble;

const QString flickrApiKey( "620131a1b82b000c9582b94effcdc636" );

PhotoPluginModel::PhotoPluginModel( QObject *parent )
    : AbstractDataPluginModel( "photo", parent ) 
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
    
    QHash<QString,QString>::const_iterator it;
    for( it = options.constBegin(); it != options.constEnd(); ++it ) {
        url += '&';
        url += it.key();
        url += '=';
        url += it.value();
    }
    
    return QUrl( url );
}

QUrl PhotoPluginModel::descriptionFileUrl( GeoDataLatLonAltBox *box,
                                           MarbleDataFacade *facade,
                                           qint32 number )
{
    // Flickr only supports images for earth
    if( facade->target() != "earth" ) {
        return QUrl();
    }
    
    QString bbox( "" );
    bbox += QString::number( box->west()  * RAD2DEG ) + ',';
    bbox += QString::number( box->south()  * RAD2DEG ) + ',';
    bbox += QString::number( box->east() * RAD2DEG ) + ',';
    bbox += QString::number( box->north() * RAD2DEG );
    
    QHash<QString,QString> options;
    options.insert( "per_page", QString::number( number ) );
    options.insert( "bbox",     bbox );
    
    return generateUrl( "flickr", "flickr.photos.search", options );
}

void PhotoPluginModel::parseFile( const QByteArray& file ) {
    QList<AbstractDataPluginItem*> list;
    FlickrParser parser( &list, this );
    
    parser.read( file );
    
    QList<AbstractDataPluginItem*>::iterator it;
    
    for( it = list.begin(); it != list.end(); ++it ) {
        if( itemExists( (*it)->id() ) ) {
            delete (*it);
            continue;
        }
        
        // Currently all Flickr images with geotags are on earth
        (*it)->setTarget( "earth" );
        downloadItemData( ((PhotoPluginItem*) (*it))->photoUrl(), "thumbnail", (*it) );
        downloadItemData( ((PhotoPluginItem*) (*it))->infoUrl(),  "info",      (*it) );
    }
}

#include "PhotoPluginModel.moc"
