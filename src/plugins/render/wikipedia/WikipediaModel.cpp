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
#include "WikipediaModel.h"

// Plugin
#include "GeonamesParser.h"

// Marble
#include "GeoDataLatLonAltBox.h"
#include "global.h"
#include "MarbleModel.h"
#include "MarbleDirs.h"
#include "WikipediaItem.h"
#include "MarbleLocale.h"
#include "MarbleDebug.h"

// Qt
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtSvg/QSvgRenderer>

using namespace Marble;

WikipediaModel::WikipediaModel( const PluginManager *pluginManager,
                                QObject *parent )
    : AbstractDataPluginModel( "wikipedia", pluginManager, parent ),
      m_showThumbnail( true )
{
    m_wikipediaIcon.addFile( MarbleDirs::path( "svg/wikipedia_shadow.svg" ) );

    m_languageCode = MarbleLocale::languageCode();
}

WikipediaModel::~WikipediaModel()
{
}

void WikipediaModel::setShowThumbnail( bool show )
{
    m_showThumbnail = show;
}

void WikipediaModel::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                         const MarbleModel *model,
                                         qint32 number )
{
    // Geonames only supports wikipedia articles for earth
    if ( model->planetId() != "earth" ) {
        return;
    }
        
    QString geonamesUrl( "http://ws.geonames.org/wikipediaBoundingBox" );
    geonamesUrl += "?north=";
    geonamesUrl += QString::number( box.north() * RAD2DEG );
    geonamesUrl += "&south=";
    geonamesUrl += QString::number( box.south() * RAD2DEG );
    geonamesUrl += "&east=";
    geonamesUrl += QString::number( box.east() * RAD2DEG );
    geonamesUrl += "&west=";
    geonamesUrl += QString::number( box.west() * RAD2DEG );
    geonamesUrl += "&maxRows=";
    geonamesUrl += QString::number( number );
    geonamesUrl += "&lang=";
    geonamesUrl += m_languageCode;
    
    downloadDescriptionFile( QUrl( geonamesUrl ) );
}

void WikipediaModel::parseFile( const QByteArray& file )
{
    QList<WikipediaItem*> list;
    GeonamesParser parser( &list, this );
    
    parser.read( file );
    
    QList<WikipediaItem*>::iterator it;
    
    for ( it = list.begin(); it != list.end(); ++it ) {
        if ( itemExists( (*it)->id() ) ) {
            delete *it;
            continue;
        }

        (*it)->setIcon( m_wikipediaIcon );
        // Currently all wikipedia articles with geotags are on earth
        (*it)->setTarget( "earth" );
        QUrl thumbnailImageUrl = (*it)->thumbnailImageUrl();
        if ( m_showThumbnail && !thumbnailImageUrl.isEmpty() ) {
            downloadItemData( thumbnailImageUrl, "thumbnail", *it );
        }
        else {
            addItemToList( *it );
        }
    }

    emit itemsUpdated();
}

#include "WikipediaModel.moc"
