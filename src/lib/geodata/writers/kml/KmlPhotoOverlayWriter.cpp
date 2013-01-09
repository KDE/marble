//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyatf@gmail.com>
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlPhotoOverlayWriter.h"

#include "GeoDataPhotoOverlay.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataPhotoOverlayType,
				 kml::kmlTag_nameSpace22 ),
    new KmlPhotoOverlayWriter );

KmlPhotoOverlayWriter::KmlPhotoOverlayWriter() : KmlOverlayTagWriter( kml::kmlTag_PhotoOverlay )
{
    // nothing to do
}

bool KmlPhotoOverlayWriter::writeMid( const GeoNode *node, GeoWriter &writer ) const
{
    const GeoDataPhotoOverlay *photo_overlay =
        static_cast<const GeoDataPhotoOverlay*>( node );

    writeElement( &photo_overlay->point(), writer );
    return true;
}

}
