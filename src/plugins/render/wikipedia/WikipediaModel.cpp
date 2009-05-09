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
#include "WikipediaWidget.h"

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
const int wikipediaIconWidth = 47;
const int wikipediaIconHeight = 40;
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
    m_wikipediaPixmap = new QPixmap( QPixmap::fromImage( wikipediaImage ) );
    
    QImage wikipediaSmallImage( wikipediaSmallIconSize,
                                wikipediaSmallIconSize,
                                QImage::Format_ARGB32 );
    wikipediaSmallImage.fill( QColor( 0, 0, 0, 0 ).rgba() );
    QPainter smallPainter( &wikipediaSmallImage );
    svgObj.render( &smallPainter );
    m_wikipediaIcon = new QIcon( QPixmap::fromImage( wikipediaSmallImage ) );
}

WikipediaModel::~WikipediaModel() {
    delete m_wikipediaPixmap;
}

QUrl WikipediaModel::descriptionFileUrl( GeoDataLatLonAltBox *box,
                                         MarbleDataFacade *facade,
                                         qint32 number )
{
    // Geonames only supports wikipedia articles for earth
    if( facade->target() != "earth" ) {
        return QUrl();
    }
        
    QString geonamesUrl( "http://ws.geonames.org/wikipediaBoundingBox" );
    geonamesUrl += "?north=";
    geonamesUrl += QString::number( box->north() * RAD2DEG );
    geonamesUrl += "&south=";
    geonamesUrl += QString::number( box->south() * RAD2DEG );
    geonamesUrl += "&east=";
    geonamesUrl += QString::number( box->east() * RAD2DEG );
    geonamesUrl += "&west=";
    geonamesUrl += QString::number( box->west() * RAD2DEG );
    geonamesUrl += "&maxRows=";
    geonamesUrl += QString::number( number );
    
    return QUrl( geonamesUrl );
}

void WikipediaModel::parseFile( QByteArray file ) {
    QList<WikipediaWidget*> list;
    GeonamesParser parser( &list, this );
    
    parser.read( file );
    
    QList<WikipediaWidget*>::iterator it;
    
    for( it = list.begin(); it != list.end(); ++it ) {
        if( widgetExists( (*it)->id() ) ) {
            delete (*it);
            continue;
        }
        
        (*it)->setPixmap( m_wikipediaPixmap );
        (*it)->setIcon( m_wikipediaIcon );
        // Currently all wikipedia articles with geotags are on earth
        (*it)->setTarget( "earth" );
        addWidgetToList( (*it) );
    }
}

#include "WikipediaModel.moc"
