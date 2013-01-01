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
#include "GeoDataPhotoOverlay.h"
#include "GeoParser.h"
#include "MarbleGlobal.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( coordinates )

static const bool kmlStrictSpecs = false;

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

    if( parentItem.represents( kmlTag_Point )
     || parentItem.represents( kmlTag_LineString )
     || parentItem.represents( kmlTag_MultiGeometry )
     || parentItem.represents( kmlTag_LinearRing ) ) {
        QStringList  coordinatesLines;// = parser.readElementText().trimmed().split( QRegExp("\\s"), QString::SkipEmptyParts );
        // Splitting using the "\\s" regexp is slow, split manually instead.
        QString text = parser.readElementText().trimmed();

        if ( !kmlStrictSpecs ) {
            // Removing spaces before and after commas
            for ( int i = 1; i < text.size() - 1; ++i ) {
                if ( text[i] == ',' ) {
                    // Before
                    int l = i - 1;
                    while ( l > 0 && text[l].isSpace() ) {
                        --l;
                    }

                    // After
                    int r = i + 1;
                    while ( r < text.size() && text[r].isSpace() ) {
                        ++r;
                    }

                    text.remove( l + 1, r - l - 1 ).insert( l + 1, ',' );
                }
            }
        }

        int index = 0;
        bool inside = true;
        int const size = text.size();
        for ( int i=0; i<size; ++i ) {
            if ( text[i].isSpace() ) {
                if ( inside ) {
                    coordinatesLines.append( text.mid( index, i-index ) );
                    inside = false;
                }
                index = i+1;
            } else {
                inside = true;
            }
        }
        coordinatesLines.append( text.mid( index ) );
        Q_FOREACH( const QString& line, coordinatesLines ) {
            QStringList coordinates = line.trimmed().split( ',' );
            if ( parentItem.represents( kmlTag_Point ) && parentItem.is<GeoDataFeature>() ) {
                GeoDataCoordinates coord;
                if ( coordinates.size() == 2 ) {
                    coord.set( coordinates.at( 0 ).toDouble(),
                              coordinates.at( 1 ).toDouble(), 0.0, GeoDataCoordinates::Degree );
                } else if( coordinates.size() == 3 ) {
                    coord.set( coordinates.at( 0 ).toDouble(),
                               coordinates.at( 1 ).toDouble(),
                               coordinates.at( 2 ).toDouble(),
                               GeoDataCoordinates::Degree );
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
                    GeoDataPoint *point = new GeoDataPoint( coord );
                    parentItem.nodeAs<GeoDataMultiGeometry>()->append( point );
                } else if ( parentItem.represents( kmlTag_Point ) ) {
                    // photo overlay
                    parentItem.nodeAs<GeoDataPoint>()->setCoordinates( coord );
                } else {
                    // raise warning as coordinates out of valid parents found
                }
            }
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << parser.name()
                 << ">" << coordinatesLines
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS

        }
    }

    if( parentItem.represents( kmlTag_Track ) ) {
        QString input = parser.readElementText().trimmed();
        if ( !kmlStrictSpecs ) {
            input = input.replace( QRegExp( "\\s*,\\s*" ), "," );
        }
        QStringList coordinates = input.split( ' ' );

        GeoDataCoordinates coord;
        if ( coordinates.size() == 2 ) {
            coord.set( DEG2RAD * coordinates.at( 0 ).toDouble(),
                       DEG2RAD * coordinates.at( 1 ).toDouble() );
        } else if( coordinates.size() == 3 ) {
            coord.set( DEG2RAD * coordinates.at( 0 ).toDouble(),
                       DEG2RAD * coordinates.at( 1 ).toDouble(),
                       coordinates.at( 2 ).toDouble() );
        }
        parentItem.nodeAs<GeoDataTrack>()->appendCoordinates( coord );
    }

    return 0;
}

}
}
