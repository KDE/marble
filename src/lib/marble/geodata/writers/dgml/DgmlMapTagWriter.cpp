// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlMapTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneMap.h"
#include "GeoSceneLayer.h"
#include "DgmlElementDictionary.h"

#include <QColor>

namespace Marble
{

static GeoTagWriterRegistrar s_writerMap( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneMapType, dgml::dgmlTag_nameSpace20 ),
                                               new DgmlMapTagWriter() );

bool DgmlMapTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneMap *map = static_cast<const GeoSceneMap*>( node );
    writer.writeStartElement( dgml::dgmlTag_Map );
    writer.writeAttribute( "bgcolor", map->backgroundColor().name() );
    writer.writeAttribute( "labelColor", map->labelColor().name() );    
    
    writer.writeStartElement( "canvas" );
    writer.writeEndElement();
    
    writer.writeStartElement( "target" );
    writer.writeEndElement();
    
    for( int i = 0; i < map->layers().count(); ++i )
    {
        writeElement( map->layers().at( i ), writer );
    }
    
    writer.writeEndElement();
    return true;
}

}
