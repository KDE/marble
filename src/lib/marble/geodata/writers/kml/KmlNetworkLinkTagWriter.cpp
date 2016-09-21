//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Adrian Draghici <draghici.adrian.b@gmail.com>
//

#include "KmlNetworkLinkTagWriter.h"

#include "GeoDataNetworkLink.h"
#include "GeoDataTypes.h"
#include "GeoDataLink.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerNetworkLink(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataNetworkLinkType,
                                     kml::kmlTag_nameSpaceOgc22 ),
        new KmlNetworkLinkTagWriter );

bool KmlNetworkLinkTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{

    const GeoDataNetworkLink *networkLink = static_cast<const GeoDataNetworkLink*>( node );

    writer.writeStartElement( kml::kmlTag_NetworkLink );

    writer.writeOptionalElement( kml::kmlTag_name, networkLink->name() );

    writer.writeOptionalElement( kml::kmlTag_visibility, QString::number( networkLink->isVisible() ), "1");

    writer.writeOptionalElement( kml::kmlTag_refreshVisibility, QString::number( networkLink->refreshVisibility() ), "0" );

    writer.writeOptionalElement( kml::kmlTag_flyToView, QString::number( networkLink->flyToView() ), "0" );

    writeElement( &networkLink->link(), writer);

    writer.writeEndElement();

    return true;
}

}
