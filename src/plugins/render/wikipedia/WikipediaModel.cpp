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
#include "WikipediaWidget.h"

// Qt
#include <QtCore/QUrl>
#include <QtCore/QString>

using namespace Marble;

WikipediaModel::WikipediaModel( QObject *parent )
    : AbstractDataPluginModel( "wikipedia", parent ) 
{
}

QUrl WikipediaModel::descriptionFileUrl( GeoDataLatLonAltBox *box, qint32 number ) {
    // FIXME: Download a list of constant number, because the parser doesn't support
    // loading a file of an unknown length.
    // http://ws.geonames.org/wikipediaBoundingBox?north=44.1&south=-9.9&east=-22.4&west=55.2
    QString geonamesUrl( "http://ws.geonames.org/wikipediaBoundingBox" );
    geonamesUrl += "?north=";
    geonamesUrl += QString::number( box->north() );
    geonamesUrl += "&south=";
    geonamesUrl += QString::number( box->south() );
    geonamesUrl += "&east=";
    geonamesUrl += QString::number( box->east() );
    geonamesUrl += "&west=";
    geonamesUrl += QString::number( box->east() );
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
        
        addWidgetToList( (*it) );
    }
}

#include "WikipediaModel.moc"
