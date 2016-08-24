//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

// Self
#include "PhotoPluginModel.h"

// Photo Plugin
#include "FlickrParser.h"
#include "PhotoPluginItem.h"
#include "PhotoPlugin.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"
#include "MarbleWidget.h"

// Qt
#include <QHash>
#include <QString>
#include <QUrl>

using namespace Marble;

const QString flickrApiKey( "620131a1b82b000c9582b94effcdc636" );

PhotoPluginModel::PhotoPluginModel( const MarbleModel *marbleModel, QObject *parent )
    : AbstractDataPluginModel( "photo", marbleModel, parent ),
      m_marbleWidget( 0 )
{
}

QUrl PhotoPluginModel::generateUrl( const QString& service,
                                    const QString& method,
                                    const QHash<QString,QString>& options )
{
    QString url;

    if (service == QLatin1String("flickr"))
        url += QLatin1String("https://www.flickr.com/services/rest/");
    else
        return QUrl();

    url += QLatin1String("?method=") + method +
           QLatin1String("&format=rest") +
           QLatin1String("&api_key=") + flickrApiKey;

    QHash<QString,QString>::const_iterator it = options.constBegin();
    QHash<QString,QString>::const_iterator const end = options.constEnd();
    for (; it != end; ++it ) {
        url += QLatin1Char('&') + it.key() + QLatin1Char('=') + it.value();
    }
    
    return QUrl( url );
}

void PhotoPluginModel::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                           qint32 number )
{
    // Flickr only supports images for earth
    if (marbleModel()->planetId() != QLatin1String("earth")) {
        return;
    }

    if( box.west() <= box.east() ) {
        const QString bbox =
            QString::number(box.west() * RAD2DEG) + QLatin1Char(',') +
            QString::number(box.south() * RAD2DEG) + QLatin1Char(',') +
            QString::number(box.east() * RAD2DEG) + QLatin1Char(',') +
            QString::number(box.north() * RAD2DEG);
    
        QHash<QString,QString> options;
        options.insert( "per_page", QString::number( number ) );
        options.insert( "bbox",     bbox );
        options.insert( "sort",     "interestingness-desc" );
        options.insert( "license", m_licenses );
    
        downloadDescriptionFile( generateUrl( "flickr", "flickr.photos.search", options ) );
    }
    else {
        // Flickr api doesn't support bboxes with west > east so we have to split in two boxes
        const QString bboxWest =
            QString::number(box.west() * RAD2DEG) + QLatin1Char(',') +
            QString::number(box.south() * RAD2DEG) + QLatin1Char(',') +
            QString::number(180 ) + QLatin1Char(',') +
            QString::number(box.north() * RAD2DEG);
        
        QHash<QString,QString> optionsWest;
        optionsWest.insert( "per_page", QString::number( number/2 ) );
        optionsWest.insert( "bbox",     bboxWest );
        optionsWest.insert( "sort",     "interestingness-desc" );
        optionsWest.insert( "license", m_licenses );

        downloadDescriptionFile( generateUrl( "flickr", "flickr.photos.search", optionsWest ) );
        
        
        const QString bboxEast =
            QString::number(-180) +QLatin1Char( ',') +
            QString::number(box.south() * RAD2DEG) + QLatin1Char(',') +
            QString::number(box.east() * RAD2DEG) + QLatin1Char(',') +
            QString::number(box.north() * RAD2DEG);

        QHash<QString,QString> optionsEast;
        optionsEast.insert( "per_page", QString::number( number/2 ) );
        optionsEast.insert( "bbox",     bboxEast );
        optionsEast.insert( "sort",     "interestingness-desc" );
        optionsEast.insert( "license", m_licenses );

        downloadDescriptionFile( generateUrl( "flickr", "flickr.photos.search", optionsEast ) );
    }
}

void PhotoPluginModel::parseFile( const QByteArray& file )
{
    QList<PhotoPluginItem*> list;
    FlickrParser parser( m_marbleWidget, &list, this );
    
    parser.read( file );
    
    QList<PhotoPluginItem*>::iterator it;
    QList<AbstractDataPluginItem*> items;
    
    for( it = list.begin(); it != list.end(); ++it ) {
        if( itemExists( (*it)->id() ) ) {
            delete (*it);
            continue;
        }

        downloadItem( (*it)->photoUrl(), "thumbnail", (*it) );
        downloadItem( (*it)->infoUrl(),  "info",      (*it) );
        items << *it;
    }
    addItemsToList( items );
}

void PhotoPluginModel::setMarbleWidget( MarbleWidget *widget )
{
    m_marbleWidget = widget;
}

void PhotoPluginModel::setLicenseValues( const QString &licenses )
{
    m_licenses = licenses;
}

#include "moc_PhotoPluginModel.cpp"
