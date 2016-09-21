//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyatf@gmail.com>
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlGroundOverlayWriter.h"

#include "GeoDataLatLonQuad.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataGroundOverlayType,
				 kml::kmlTag_nameSpaceOgc22 ),
    new KmlGroundOverlayWriter );

KmlGroundOverlayWriter::KmlGroundOverlayWriter() : KmlOverlayTagWriter( kml::kmlTag_GroundOverlay )
{
    // nothing to do
}

bool KmlGroundOverlayWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    KmlOverlayTagWriter::writeMid( node, writer );

    const GeoDataGroundOverlay *ground_overlay =
        static_cast<const GeoDataGroundOverlay*>( node );

    writer.writeOptionalElement( kml::kmlTag_altitude,
                                 QString::number(ground_overlay->altitude()), "0" );
    KmlGroundOverlayWriter::writeAltitudeMode( writer, ground_overlay->altitudeMode() );

    if ( !ground_overlay->latLonBox().isEmpty() ) {
        writeElement( &ground_overlay->latLonBox(), writer );
    }

    if ( ground_overlay->latLonQuad().isValid() ) {
        writeElement( &ground_overlay->latLonQuad(), writer );
    }

    return true;
}

QString KmlGroundOverlayWriter::altitudeModeToString(AltitudeMode mode)
{
    switch (mode) {
    case ClampToGround:
    return "clampToGround";
    case RelativeToGround:
    return "relativeToGround";
    case ClampToSeaFloor:
    return "clampToSeaFloor";
    case RelativeToSeaFloor:
    return "relativeToSeaFloor";
    case Absolute:
    return "absolute";
    }
    return "";
}

void KmlGroundOverlayWriter::writeAltitudeMode(GeoWriter& writer, AltitudeMode altMode)
{
    if ( altMode == ClampToGround ) {
        // clampToGround is always the default value, so we never have to write it
        return;
    }

    const QString altitudeMode = KmlGroundOverlayWriter::altitudeModeToString( altMode );
    bool const isGoogleExtension = ( altMode == ClampToSeaFloor || altMode == RelativeToSeaFloor );
    if ( isGoogleExtension ) {
        // clampToSeaFloor and relativeToSeaFloor are Google extensions that need a gx: tag namespace
        writer.writeElement( kml::kmlTag_nameSpaceGx22, kml::kmlTag_altitudeMode, altitudeMode);
    } else {
        writer.writeElement( kml::kmlTag_altitudeMode, altitudeMode );
    }
}

}

