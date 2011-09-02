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
#include <QtCore/QString>

#include "MarbleDebug.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "osm-namefinder/AttributeDictionary.h"
#include "osm-namefinder/ElementDictionary.h"

namespace Marble
{
namespace OsmNamefinder
{

static GeoTagHandlerRegistrar
searchTagHandler( GeoParser::QualifiedName( tag_searchresults, tag_namespace ),
         new SearchResultsTagHandler );


GeoNode * SearchResultsTagHandler::parse( GeoParser & parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( tag_searchresults ));
    mDebug() << "SearchResultsTagHandler";

    // FIXME:
    GeoDocument * const document = parser.activeDocument();
    GeoDataDocument * const searchResults =
        dynamic_cast<GeoDataDocument * const>( document );
    Q_ASSERT( searchResults );
    return searchResults;
}

}
}
