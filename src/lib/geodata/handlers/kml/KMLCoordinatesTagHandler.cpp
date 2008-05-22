/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

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

#include "KMLCoordinatesTagHandler.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include "KMLElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( coordinates )

KMLcoordinatesTagHandler::KMLcoordinatesTagHandler()
    : GeoTagHandler()
{
}

KMLcoordinatesTagHandler::~KMLcoordinatesTagHandler()
{
}

GeoNode* KMLcoordinatesTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_coordinates ) );

    GeoStackItem parentItem = parser.parentElement();
    
    if( parentItem.represents( kmlTag_Point ) ) {
        QStringList coordinates;
        coordinates = parser.readElementText().trimmed().split( "," );
        
        if( coordinates.size() == 2 ) {
            // assume <coordinates>lat,lon</coordinates>
            parentItem.nodeAs<GeoDataPlacemark>()->setCoordinate( coordinates.at( 0 ).toDouble(), 
                                                                  coordinates.at( 1 ).toDouble() );
        } else if( coordinates.size() == 3 ) {
            // assume <coordinates>lat,lon,alt</coordinates>
            parentItem.nodeAs<GeoDataPlacemark>()->setCoordinate( coordinates.at( 0 ).toDouble(), 
                                                                  coordinates.at( 1 ).toDouble(), 
                                                                  coordinates.at( 2 ).toDouble() );
        } else {
            // raise warning
        }
        qDebug() << "Parsed <coordinates> start! Added  item: " << coordinates 
                 << " parent item name: " << parentItem.qualifiedName().first 
                 << " associated item: " << parentItem.associatedNode();
    }

    return 0;
}
