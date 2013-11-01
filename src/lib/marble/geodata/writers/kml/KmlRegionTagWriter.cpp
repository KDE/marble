//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlRegionTagWriter.h"

#include "GeoDataRegion.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "GeoTagWriter.h"
#include "KmlElementDictionary.h"
#include "KmlLatLonAltBoxWriter.h"
#include "KmlLodTagWriter.h"

namespace Marble
{

bool KmlRegionTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataRegion *region = static_cast<const GeoDataRegion*>( node );
    writer.writeStartElement( kml::kmlTag_Region );
    writeElement( &region->latLonAltBox(), writer );
    writeElement( &region->lod(), writer );
    writer.writeEndElement();
    return true;
}

}

