//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlSectionTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneSection.h"
#include "GeoSceneItem.h"
#include "GeoSceneIcon.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSection( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneSectionType, dgml::dgmlTag_nameSpace20 ),
                                                                            new DgmlSectionTagWriter() );

bool DgmlSectionTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneSection *section = static_cast<const GeoSceneSection*>( node );
    
    writer.writeStartElement( dgml::dgmlTag_Section );
    writer.writeAttribute( "name", section->name() );
    writer.writeAttribute( "checkable", section->checkable() ? "true" : "false" );
    writer.writeAttribute( "connect", section->connectTo() );
    writer.writeAttribute( "spacing", QString::number( section->spacing() ) );
    writer.writeElement( dgml::dgmlTag_Heading, section->heading() );
    
    for( int i = 0; i < section->items().count(); ++i )
    {
        GeoSceneItem *item = section->items().at( i );
        writeElement( item, writer );
    }
    
    writer.writeEndElement();
    return true;
}

}
