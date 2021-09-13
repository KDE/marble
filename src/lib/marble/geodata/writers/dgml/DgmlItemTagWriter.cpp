// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
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
    } else if (item->icon()->color().isValid()) {
        writer.writeAttribute( "color", item->icon()->color().name() );
    }
    writer.writeEndElement();
    writer.writeElement( "text", item->text() );
    
    writer.writeEndElement();
    return true;
}

}
