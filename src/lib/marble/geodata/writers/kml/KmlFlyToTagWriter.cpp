
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

bool KmlFlyToTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataFlyTo *flyTo = static_cast<const GeoDataFlyTo*>( node );
    writer.writeStartElement( kml::kmlTag_nameSpaceGx22, kml::kmlTag_FlyTo );
    writer.writeElement( kml::kmlTag_nameSpaceGx22, kml::kmlTag_duration, QString::number( flyTo->duration()) );
    QString const flyToModeString = flyTo->flyToMode() == GeoDataFlyTo::Smooth ? "smooth" : "bounce";
    writer.writeElement( kml::kmlTag_nameSpaceGx22, kml::kmlTag_flyToMode, flyToModeString );
    writeElement( flyTo->view(), writer );
    writer.writeEndElement();
    return true;
}

}
