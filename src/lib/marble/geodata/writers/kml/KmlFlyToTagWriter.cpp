
//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlFlyToTagWriter.h"

#include "GeoDataFlyTo.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerFlyTo(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataFlyToType,
                                     kml::kmlTag_nameSpace22 ),
        new KmlFlyToTagWriter );

bool KmlFlyToTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataFlyTo *flyTo = static_cast<const GeoDataFlyTo*>( node );
    writer.writeStartElement( kml::kmlTag_nameSpaceGx22, kml::kmlTag_FlyTo );
    writer.writeElement( kml::kmlTag_nameSpaceGx22, kml::kmlTag_duration, QString::number( flyTo->duration()) );
    if ( flyTo->flyToMode() == GeoDataFlyTo::Smooth ) {
        // two values, smooth and bounce, bounce is default and can hence be omitted
        writer.writeElement( kml::kmlTag_nameSpaceGx22, kml::kmlTag_flyToMode, "smooth" );
    }
    if ( flyTo->view() ) {
        writeElement( flyTo->view(), writer );
    }
    writer.writeEndElement();
    return true;
}

}
