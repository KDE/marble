//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#include "OsmObjectAttributeWriter.h"
#include "osm/OsmObjectManager.h"
#include "osm/OsmPlacemarkData.h"
#include "GeoWriter.h"

namespace Marble {

void OsmObjectAttributeWriter::writeAttributes( const OsmPlacemarkData& osmData, GeoWriter &writer )
{
    writer.writeAttribute( "id", QString::number( osmData.id() ) );
    writer.writeOptionalAttribute( "action", osmData.action() );
    writer.writeOptionalAttribute( "changeset", osmData.changeset() );
    writer.writeOptionalAttribute( "timestamp", osmData.timestamp() );
    writer.writeOptionalAttribute( "uid", osmData.uid() );
    writer.writeOptionalAttribute( "user", osmData.user() );
    writer.writeOptionalAttribute( "version", osmData.version() );
    writer.writeOptionalAttribute( "visible", osmData.isVisible() );
}

}

