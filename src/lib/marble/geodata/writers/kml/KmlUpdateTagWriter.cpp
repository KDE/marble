//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlUpdateTagWriter.h"
#include "GeoDataUpdate.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "GeoDataAbstractView.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerUpdate(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataUpdateType,
                                     kml::kmlTag_nameSpace22 ),
        new KmlUpdateTagWriter );

bool KmlUpdateTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataUpdate *update = static_cast<const GeoDataUpdate*>( node );
    writer.writeStartElement( kml::kmlTag_Update );
    writer.writeElement( kml::kmlTag_targetHref, update->targetHref() );
    writer.writeEndElement();
    return true;
}

}
