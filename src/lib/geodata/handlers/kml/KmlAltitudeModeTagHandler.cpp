/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>
    Copyright (C) 2009 Bastian Holst <bastianholst@gmx.de>

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

#include "KmlAltitudeModeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataGeometry.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataPoint.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTrack.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( altitudeMode )

GeoNode* KmlaltitudeModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_altitudeMode ) );

    QString content = parser.readElementText().trimmed();

    AltitudeMode mode;
    if( content == QString( "relativeToGround" ) ) {
        mode = RelativeToGround;
    }
    else if( content == QString( "absolute" ) ) {
        mode = Absolute;
    }
    else { // clampToGround is Standard
        mode = ClampToGround;
    }

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataPlacemark>()
         && parentItem.represents( kmlTag_Point ) )
    {
         parentItem.nodeAs<GeoDataPlacemark>()->geometry()->setAltitudeMode( mode );
    }
    else if ( parentItem.is<GeoDataPoint>() )
    {
        parentItem.nodeAs<GeoDataPoint>()->setAltitudeMode( mode );
    }
    else if ( parentItem.is<GeoDataLatLonAltBox>() ) {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setAltitudeMode( mode );
    }
    else if ( parentItem.is<GeoDataTrack>() ) {
        parentItem.nodeAs<GeoDataTrack>()->setAltitudeMode( mode );
    }
    else if ( parentItem.is<GeoDataGroundOverlay>() ) {
        parentItem.nodeAs<GeoDataGroundOverlay>()->setAltitudeMode( mode );
    }

#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_altitudeMode << "> containing: " << content
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif
    return 0;
}

}
}
