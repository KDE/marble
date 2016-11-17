//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

//Self
#include "OsmTagTagWriter.h"

//Marble
#include "GeoWriter.h"
#include "OsmElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

QSet<QString> OsmTagTagWriter::m_blacklist;

void OsmTagTagWriter::writeTags( const OsmPlacemarkData& osmData, GeoWriter &writer )
{
    if (m_blacklist.isEmpty()) {
        m_blacklist << QStringLiteral("mx:version");
        m_blacklist << QStringLiteral("mx:changeset");
        m_blacklist << QStringLiteral("mx:uid");
        m_blacklist << QStringLiteral("mx:visible");
        m_blacklist << QStringLiteral("mx:user");
        m_blacklist << QStringLiteral("mx:timestamp");
        m_blacklist << QStringLiteral("mx:action");
    }

    auto it = osmData.tagsBegin();
    auto end = osmData.tagsEnd();

    for ( ; it != end; ++it ) {
        if (!m_blacklist.contains(it.key())) {
            writer.writeStartElement( osm::osmTag_tag );
            writer.writeAttribute( "k", it.key() );
            writer.writeAttribute( "v", it.value() );
            writer.writeEndElement();
        }
    }
}

}
