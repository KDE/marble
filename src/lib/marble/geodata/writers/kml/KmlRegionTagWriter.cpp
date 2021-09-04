//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlRegionTagWriter.h"

#include "GeoDataRegion.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"
#include "GeoDataLod.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlLatLonAltBoxWriter.h"
#include "KmlLodTagWriter.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerRegion(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataRegionType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlRegionTagWriter);

bool KmlRegionTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataRegion *region = static_cast<const GeoDataRegion*>( node );
    writer.writeStartElement( kml::kmlTag_Region );
    KmlObjectTagWriter::writeIdentifiers( writer, region );
    writeElement( &region->latLonAltBox(), writer );
    writeElement( &region->lod(), writer );
    writer.writeEndElement();
    return true;
}

}

