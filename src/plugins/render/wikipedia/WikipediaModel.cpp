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
#include "MarbleGlobal.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleDirs.h"
#include "WikipediaItem.h"
#include "MarbleLocale.h"
#include "MarbleDebug.h"

// Qt
#include <QUrl>
#include <QString>
#include <QPainter>

#include <QUrlQuery>

using namespace Marble;

WikipediaModel::WikipediaModel( const MarbleModel *marbleModel, QObject *parent )
    : AbstractDataPluginModel( "wikipedia", marbleModel, parent ),
      m_marbleWidget( 0 ),
      m_wikipediaIcon(MarbleDirs::path(QStringLiteral("svg/wikipedia_shadow.svg"))),
      m_showThumbnail( true )
{
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
                                         qint32 number )
{
    // Geonames only supports wikipedia articles for earth
    if (marbleModel()->planetId() != QLatin1String("earth")) {
        return;
    }
        
    QUrl geonamesUrl( "http://ws.geonames.org/wikipediaBoundingBox" );
    QUrlQuery urlQuery;
    urlQuery.addQueryItem( "north", QString::number( box.north( GeoDataCoordinates::Degree ) ) );
    urlQuery.addQueryItem( "south", QString::number( box.south( GeoDataCoordinates::Degree ) ) );
    urlQuery.addQueryItem( "east", QString::number( box.east( GeoDataCoordinates::Degree ) ) );
    urlQuery.addQueryItem( "west", QString::number( box.west( GeoDataCoordinates::Degree ) ) );
    urlQuery.addQueryItem( "maxRows", QString::number( number ) );
    urlQuery.addQueryItem( "lang", m_languageCode );
    urlQuery.addQueryItem( "username", "marble" );
    geonamesUrl.setQuery( urlQuery );

    downloadDescriptionFile( geonamesUrl );
}

void WikipediaModel::parseFile( const QByteArray& file )
{
    QList<WikipediaItem*> list;
    GeonamesParser parser( m_marbleWidget, &list, this );
    
    parser.read( file );
    
    QList<AbstractDataPluginItem*> items;
    QList<WikipediaItem*>::const_iterator it;
    
    for ( it = list.constBegin(); it != list.constEnd(); ++it ) {
        if ( itemExists( (*it)->id() ) ) {
            delete *it;
            continue;
        }

        (*it)->setIcon( m_wikipediaIcon );
        QUrl thumbnailImageUrl = (*it)->thumbnailImageUrl();
        if ( m_showThumbnail && !thumbnailImageUrl.isEmpty() ) {
            downloadItem( thumbnailImageUrl, "thumbnail", *it );
        }
        else {
            items << *it;
        }
    }

    addItemsToList( items );
}

void WikipediaModel::setMarbleWidget(MarbleWidget *widget)
{
    m_marbleWidget = widget;
}

#include "moc_WikipediaModel.cpp"
