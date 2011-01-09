//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlItemTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneSection.h"
#include "GeoSceneItem.h"
#include "GeoSceneIcon.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerItem( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneItemType, dgml::dgmlTag_nameSpace20 ),
                                                                            new DgmlItemTagWriter() );

bool DgmlItemTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneItem *item = static_cast<const GeoSceneItem*>( node );
    
    writer.writeStartElement( dgml::dgmlTag_Item );
    writer.writeAttribute( "name", item->name() );
    
    writer.writeStartElement( dgml::dgmlTag_Icon );
    if( !item->icon()->pixmap().isEmpty() ) {
        writer.writeAttribute( "pixmap", item->icon()->pixmap() );
    } else if( !item->icon()->color().name().isEmpty() ) {
        writer.writeAttribute( "color", item->icon()->color().name() );
    }
    writer.writeEndElement();
    writer.writeElement( "text", item->text() );
    
    writer.writeEndElement();
    return true;
}

};