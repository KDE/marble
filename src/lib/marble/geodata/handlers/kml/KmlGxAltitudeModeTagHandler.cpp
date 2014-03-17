//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya  <sanjiban22393@gmail.com>
//


#include "KmlGxAltitudeModeTagHandler.h"
#include "KmlAltitudeModeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataGeometry.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataPoint.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTrack.h"
#include "GeoDataAbstractView.h"
#include "GeoDataModel.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
namespace gx
{
KML_DEFINE_TAG_HANDLER_GX22( altitudeMode )

GeoNode* KmlaltitudeModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_altitudeMode ) );

    QString content = parser.readElementText().trimmed();

    AltitudeMode mode;
    if( content == QString( "relativeToGround" ) ) {
        mode = RelativeToGround;
    } else if( content == QString( "absolute" ) ) {
        mode = Absolute;
    } else if( content == QString( "relativeToSeaFloor" ) ) {
        mode = RelativeToSeaFloor;
    } else if( content == QString( "clampToSeaFloor" ) ) {
        mode = ClampToSeaFloor;
    }else if( content == QString( "clampToGround" ) ) {
        mode = ClampToGround;
    } else {
        mDebug() << "Unknown altitude mode " << content << ", falling back to 'clampToGround'";
        mode = ClampToGround;
    }

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataPlacemark>()
         && parentItem.represents( kmlTag_Point ) ) {
         parentItem.nodeAs<GeoDataPlacemark>()->geometry()->setAltitudeMode( mode );
    } else if ( parentItem.is<GeoDataPoint>() ) {
        parentItem.nodeAs<GeoDataPoint>()->setAltitudeMode( mode );
    } else if ( parentItem.is<GeoDataLatLonAltBox>() ) {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setAltitudeMode( mode );
    } else if ( parentItem.is<GeoDataTrack>() ) {
        parentItem.nodeAs<GeoDataTrack>()->setAltitudeMode( mode );
    } else if ( parentItem.is<GeoDataGroundOverlay>() ) {
        parentItem.nodeAs<GeoDataGroundOverlay>()->setAltitudeMode( mode );
    } else if ( parentItem.is<GeoDataAbstractView>() ) {
        parentItem.nodeAs<GeoDataAbstractView>()->setAltitudeMode( mode );
    } else if ( parentItem.is<GeoDataModel>() ) {
        parentItem.nodeAs<GeoDataModel>()->setAltitudeMode( mode );
    }

    return 0;
}

}
}
}
