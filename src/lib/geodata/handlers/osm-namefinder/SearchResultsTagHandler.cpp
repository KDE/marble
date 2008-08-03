/*
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#include "osm-namefinder/SearchResultsTagHandler.h"

#include <QtCore/QDate>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QString>

#include "GeoParser.h"
#include "GeoDataContainer.h"
#include "osm-namefinder/AttributeDictionary.h"
#include "osm-namefinder/ElementDictionary.h"

namespace Marble
{
namespace OsmNamefinder
{

static GeoTagHandlerRegistrar
handler( GeoTagHandler::QualifiedName( tag_searchresults, tag_namespace ),
         new SearchResultsTagHandler );


GeoNode * SearchResultsTagHandler::parse( GeoParser & parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( tag_searchresults ));
    qDebug() << "SearchResultsTagHandler";

    // FIXME:
    GeoDocument * const document = parser.activeDocument();
    GeoDataContainer * const searchResults =
        dynamic_cast<GeoDataContainer * const>( document );
    Q_ASSERT( searchResults );

//     // attribute date, for example "2007-05-08 12:56:33"
//     const QString dateStr = parser.attribute( attr_date ).trimmed();
//     const QDateTime date = QDateTime::fromString( dateStr, "yyyy-MM-dd HH:mm:ss" );
//     Q_ASSERT( date.isValid() );
//     searchResults->setDateTimeOfSearch( date );

//     // attribute sourcedate, for example "2007-05-02"
//     const QString sourceDateStr = parser.attribute( attr_sourcedate ).trimmed();
//     const QDate sourceDate = QDate::fromString( sourceDateStr, "yyyy-MM-dd" );
//     Q_ASSERT( sourceDate.isValid() );
//     searchResults->setDateOfIndexData( sourceDate );

//     // attribute find
//     const QString find = parser.attribute( attr_find ).trimmed();
//     searchResults->setOriginalSearchRequest( find );

//     // attribute distancesearch
//     const QString distanceSearchStr = parser.attribute( attr_distancesearch ).trimmed();
//     const bool distanceSearch = ( distanceSearchStr == "yes" );
//     searchResults->setDistanceSearch( distanceSearch );

//     // attribute findname
//     const QString findName = parser.attribute( attr_findname ).trimmed();
//     searchResults->setFindName( findName );

//     // attribute findplace
//     const QString findPlace = parser.attribute( attr_findplace ).trimmed();
//     searchResults->setFindPlace( findPlace );

//     // attribute findisin
//     const QString findIsIn = parser.attribute( attr_findisin ).trimmed();
//     searchResults->setFindIsIn( findIsIn );

//     // attribute foundnearplace
//     const QString foundNearPlaceStr = parser.attribute( attr_foundnearplace ).trimmed();
//     const bool foundNearPlace = ( foundNearPlaceStr == "yes" );
//     searchResults->setFoundNearPlace( foundNearPlace );

    return searchResults;
}

}
}
