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
#include "MarbleDirs.h"
#include "WikipediaWidget.h"

// Qt
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>

using namespace Marble;

// The Wikipedia icon is not a square
const int wikipediaIconWidth = 47;
const int wikipediaIconHeight = 40;

WikipediaModel::WikipediaModel( QObject *parent )
    : AbstractDataPluginModel( "wikipedia", parent ) 
{
    // Rendering of the wikipedia icon from svg
    QSvgRenderer svgObj( MarbleDirs::path( "svg/wikipedia.svg" ), this );
    m_wikipediaImage = new QImage( wikipediaIconWidth, wikipediaIconHeight, QImage::Format_ARGB32 );
    m_wikipediaImage->fill( QColor( 0, 0, 0, 0 ).rgba() );
    QPainter painter( m_wikipediaImage );
    svgObj.render( &painter );
}

WikipediaModel::~WikipediaModel() {
    delete m_wikipediaImage;
}

QUrl WikipediaModel::descriptionFileUrl( GeoDataLatLonAltBox *box, qint32 number ) {
    // http://ws.geonames.org/wikipediaBoundingBox?north=44.1&south=-9.9&east=-22.4&west=55.2
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
        
        (*it)->setIcon( m_wikipediaImage );
        addWidgetToList( (*it) );
    }
}

#include "WikipediaModel.moc"
