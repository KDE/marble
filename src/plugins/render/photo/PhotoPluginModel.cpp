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
#include "PhotoPluginWidget.h"

// Marble
#include "AbstractDataPluginWidget.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDataFacade.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QUrl>

using namespace Marble;

PhotoPluginModel::PhotoPluginModel( QObject *parent )
    : AbstractDataPluginModel( "photo", parent ) 
{
}

QUrl PhotoPluginModel::descriptionFileUrl( GeoDataLatLonAltBox *box,
                                           MarbleDataFacade *facade,
                                           qint32 number )
{
    // Flickr only supports images for earth
    if( facade->target() != "earth" ) {
        return QUrl();
    }
    
    QString flickrUrl( "http://www.flickr.com/services/rest/" );
    flickrUrl += "?method=flickr.photos.search";
    flickrUrl += "&format=rest";
    flickrUrl += "&api_key=620131a1b82b000c9582b94effcdc636";
    flickrUrl += "&per_page=" + QString::number( number );
    flickrUrl += "&bbox=" + QString::number( box->west()  * RAD2DEG ) + ',';
    flickrUrl += QString::number( box->south()  * RAD2DEG ) + ',';
    flickrUrl += QString::number( box->east() * RAD2DEG ) + ',';
    flickrUrl += QString::number( box->north() * RAD2DEG );
    
    return QUrl( flickrUrl );
}

void PhotoPluginModel::parseFile( QByteArray file ) {
    QList<AbstractDataPluginWidget*> list;
    FlickrParser parser( &list, this );
    
    parser.read( file );
    
    QList<AbstractDataPluginWidget*>::iterator it;
    
    for( it = list.begin(); it != list.end(); ++it ) {
        if( widgetExists( (*it)->id() ) ) {
            delete (*it);
            continue;
        }
        
        // Currently all Flickr images with geotags are on earth
        (*it)->setTarget( "earth" );
        downloadWidgetData( ((PhotoPluginWidget*) (*it))->photoUrl(), "thumbnail", (*it) );
        downloadWidgetData( ((PhotoPluginWidget*) (*it))->infoUrl( "620131a1b82b000c9582b94effcdc636" ),
                            "info",
                            (*it) );
    }
}

#include "PhotoPluginModel.moc"
