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

#include "KmlCoordinatesTagHandler.h"

#include <QtCore/QStringList>
#include <QtCore/QRegExp>

#include "MarbleDebug.h"
#include "KmlElementDictionary.h"
#include "GeoDataTrack.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataMultiGeometry.h"
#include "GeoParser.h"
#include "global.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( coordinates )

// We can't use KML_DEFINE_TAG_HANDLER_GX22 because the name of the tag ("coord")
// and the TagHandler ("KmlcoordinatesTagHandler") don't match
static GeoTagHandlerRegistrar s_handlercoordkmlTag_nameSpaceGx22(GeoParser::QualifiedName(kmlTag_coord, kmlTag_nameSpaceGx22 ),
                                                                 new KmlcoordinatesTagHandler());

GeoNode* KmlcoordinatesTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement()
             && ( parser.isValidElement( kmlTag_coordinates )
                  || parser.isValidElement( kmlTag_coord ) ) );

    GeoStackItem parentItem = parser.parentElement();

    bool isParentTrack = parentItem.represents( kmlTag_Track );

    if( parentItem.represents( kmlTag_Point )
     || parentItem.represents( kmlTag_LineString )
     || parentItem.represents( kmlTag_MultiGeometry )
     || parentItem.represents( kmlTag_LinearRing )
     || isParentTrack) {
        char separator = isParentTrack ? ' ' : ',';
        QStringList coordinates = parser.readElementText().trimmed().split( separator );
        if ( parentItem.represents( kmlTag_Point ) && parentItem.is<GeoDataFeature>() ) {
            GeoDataPoint coord;
            if ( coordinates.size() == 2 ) {
                coord.set( DEG2RAD * coordinates.at( 0 ).toDouble(),
                            DEG2RAD * coordinates.at( 1 ).toDouble() );
            } else if( coordinates.size() == 3 ) {
                coord.set( DEG2RAD * coordinates.at( 0 ).toDouble(),
                            DEG2RAD * coordinates.at( 1 ).toDouble(),
                            coordinates.at( 2 ).toDouble() );
            }
            parentItem.nodeAs<GeoDataPlacemark>()->setCoordinate( coord );
        } else {
            GeoDataCoordinates coord;
            if ( coordinates.size() == 2 ) {
                coord.set( DEG2RAD * coordinates.at( 0 ).toDouble(),
                            DEG2RAD * coordinates.at( 1 ).toDouble() );
            } else if( coordinates.size() == 3 ) {
                coord.set( DEG2RAD * coordinates.at( 0 ).toDouble(),
                            DEG2RAD * coordinates.at( 1 ).toDouble(),
                            coordinates.at( 2 ).toDouble() );
            }

            if ( parentItem.represents( kmlTag_LineString ) ) {
                parentItem.nodeAs<GeoDataLineString>()->append( coord );
            } else if ( parentItem.represents( kmlTag_LinearRing ) ) {
                parentItem.nodeAs<GeoDataLinearRing>()->append( coord );
            } else if ( parentItem.represents( kmlTag_MultiGeometry ) ) {
                GeoDataPoint *point = new GeoDataPoint;
                if ( coordinates.size() == 2 ) {
                    point->set( DEG2RAD * coordinates.at( 0 ).toDouble(),
                                DEG2RAD * coordinates.at( 1 ).toDouble() );
                } else if ( coordinates.size() == 3 ) {
                    point->set( DEG2RAD * coordinates.at( 0 ).toDouble(),
                                DEG2RAD * coordinates.at( 1 ).toDouble(),
                                coordinates.at( 2 ).toDouble() );
                }
                parentItem.nodeAs<GeoDataMultiGeometry>()->append( point );
            } else if ( parentItem.represents( kmlTag_Track ) ) {
                parentItem.nodeAs<GeoDataTrack>()->appendCoordinates( coord );
            } else if ( parentItem.represents( kmlTag_Point ) ) {
/*                    mDebug() << "found a free Point!";
                qreal lon, lat;
                coord.geoCoordinates(lon, lat);
                parentItem.nodeAs<GeoDataPoint>()->set(lon, lat, coord.altitude());*/
            } else {
                // raise warning as coordinates out of valid parents found
            }
        }
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << ( isParentTrack ? kmlTag_coord : kmlTag_coordinates )
                 << "> containing: " << coordinates
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}
