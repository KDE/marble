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
#include "WikipediaModel.h"

// Plugin
#include "GeonamesParser.h"

// Marble
#include "GeoDataLatLonAltBox.h"
#include "global.h"
#include "MarbleDataFacade.h"
#include "MarbleDirs.h"
#include "WikipediaItem.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtSvg/QSvgRenderer>

using namespace Marble;

// The Wikipedia icon is not a square
const int wikipediaIconWidth = 32;
const int wikipediaIconHeight = 27;
const int wikipediaSmallIconSize = 16;

WikipediaModel::WikipediaModel( QObject *parent )
    : AbstractDataPluginModel( "wikipedia", parent ) 
{
    // Rendering of the wikipedia icon from svg
    QSvgRenderer svgObj( MarbleDirs::path( "svg/wikipedia.svg" ), this );
    QImage wikipediaImage( wikipediaIconWidth,
                           wikipediaIconHeight,
                           QImage::Format_ARGB32 );
    wikipediaImage.fill( QColor( 0, 0, 0, 0 ).rgba() );
    QPainter painter( &wikipediaImage );
    svgObj.render( &painter );
    m_wikipediaPixmap = QPixmap::fromImage( wikipediaImage );
    
    m_wikipediaIcon.addFile( MarbleDirs::path( "svg/wikipedia.svg" ) );
}

WikipediaModel::~WikipediaModel() {
}

void WikipediaModel::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                         MarbleDataFacade *facade,
                                         qint32 number )
{
    // Geonames only supports wikipedia articles for earth
    if( facade->target() != "earth" ) {
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
    
    downloadDescriptionFile( QUrl( geonamesUrl ) );
}

void WikipediaModel::parseFile( const QByteArray& file ) {
    QList<WikipediaItem*> list;
    GeonamesParser parser( &list, this );
    
    parser.read( file );
    
    QList<WikipediaItem*>::iterator it;
    
    for( it = list.begin(); it != list.end(); ++it ) {
        if( itemExists( (*it)->id() ) ) {
            delete (*it);
            continue;
        }
        
        (*it)->setPixmap( m_wikipediaPixmap );
        (*it)->setIcon( m_wikipediaIcon );
        // Currently all wikipedia articles with geotags are on earth
        (*it)->setTarget( "earth" );
        addItemToList( (*it) );
    }
}

#include "WikipediaModel.moc"
