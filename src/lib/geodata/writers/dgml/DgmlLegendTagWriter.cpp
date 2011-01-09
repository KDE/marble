//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlLegendTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSection.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLegend( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneLegendType, dgml::dgmlTag_nameSpace20 ),
                                               new DgmlLegendTagWriter() );

bool DgmlLegendTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoSceneLegend *legend = static_cast<const GeoSceneLegend*>( node );
    
    writer.writeStartElement( dgml::dgmlTag_Legend );
    for( int i = 0; i < legend->sections().count(); ++i )
    {
        GeoSceneSection *section = legend->sections().at( i );
        writeElement( section, writer );
    }
    writer.writeEndElement();
    return true;
}

}
