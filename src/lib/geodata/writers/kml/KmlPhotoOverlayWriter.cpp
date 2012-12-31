//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyatf@gmail.com>
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

bool KmlPhotoOverlayWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataPhotoOverlay *photo_overlay =
	static_cast<const GeoDataPhotoOverlay*>( node );

    writer.writeStartElement( kml::kmlTag_PhotoOverlay );

    writeElement( &photo_overlay->point(), writer );

    writer.writeEndElement();

    return true;
}

}

