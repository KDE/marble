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

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include "KmlElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataParser.h"
#include "global.h"

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( coordinates )

KmlcoordinatesTagHandler::KmlcoordinatesTagHandler()
    : GeoTagHandler()
{
}

KmlcoordinatesTagHandler::~KmlcoordinatesTagHandler()
{
}

GeoNode* KmlcoordinatesTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_coordinates ) );

    GeoStackItem parentItem = parser.parentElement();
    
    if( parentItem.nodeAs<GeoDataGeometry>() ) {
        QStringList coordinatesLines = parser.readElementText().trimmed().split( " ", QString::SkipEmptyParts );
        Q_FOREACH( const QString& line, coordinatesLines ) {
            QStringList coordinates = line.trimmed().split( "," );
            if( parentItem.represents( kmlTag_Point ) ) {
                GeoDataPoint* coord = new GeoDataPoint();
                if( coordinates.size() == 2 ) {
                    coord->set( DEG2RAD * coordinates.at( 0 ).toDouble(), 
                                DEG2RAD * coordinates.at( 1 ).toDouble() );
                } else if( coordinates.size() == 3 ) {
                    coord->set( DEG2RAD * coordinates.at( 0 ).toDouble(), 
                                DEG2RAD * coordinates.at( 1 ).toDouble(),
                                coordinates.at( 2 ).toDouble() );
                }
                parentItem.nodeAs<GeoDataPlacemark>()->setCoordinate( *coord );
                parentItem.nodeAs<GeoDataPlacemark>()->setGeometry( coord );
            } else {
                GeoDataCoordinates* coord = new GeoDataCoordinates();
                if( coordinates.size() == 2 ) {
                    coord->set( DEG2RAD * coordinates.at( 0 ).toDouble(), 
                               DEG2RAD * coordinates.at( 1 ).toDouble() );
                } else if( coordinates.size() == 3 ) {
                    coord->set( DEG2RAD * coordinates.at( 0 ).toDouble(), 
                               DEG2RAD * coordinates.at( 1 ).toDouble(),
                               coordinates.at( 2 ).toDouble() );
                }
                if( parentItem.represents( kmlTag_LineString ) ) {
                    parentItem.nodeAs<GeoDataLineString>()->append( coord );
                } else if( parentItem.represents( kmlTag_LinearRing ) ) {
                    parentItem.nodeAs<GeoDataLinearRing>()->append( coord );
                } else {
                    // raise warning as coordinates out of valid parents found
                }
            }
#ifdef DEBUG_TAGS
        qDebug() << "Parsed <" << kmlTag_coordinates << "> containing: " << coordinates
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        }
    }
    return 0;
}
