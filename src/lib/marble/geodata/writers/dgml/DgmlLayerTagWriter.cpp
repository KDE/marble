// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlLayerTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneAbstractDataset.h"
#include "GeoSceneLayer.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLayer( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneLayerType, dgml::dgmlTag_nameSpace20 ),
                                               new DgmlLayerTagWriter() );

bool DgmlLayerTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneLayer *layer = static_cast<const GeoSceneLayer*>( node );
    writer.writeStartElement( QString::fromUtf8(dgml::dgmlTag_Layer) );
    writer.writeAttribute( "name", layer->name() );
    writer.writeAttribute( "backend", layer->backend() );
    if( !layer->role().isEmpty() )
    {
        writer.writeAttribute( "role", layer->role() );
    }
    
    for( int i = 0; i < layer->datasets().count(); ++i )
    {
        writeElement( layer->datasets().at( i ), writer );
    }
    
    writer.writeEndElement();
    return true;
}

}
