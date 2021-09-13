// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlHeadTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneZoom.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerHead( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneHeadType, dgml::dgmlTag_nameSpace20 ),
                                               new DgmlHeadTagWriter() );

bool DgmlHeadTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoSceneHead *head = static_cast<const GeoSceneHead*>( node );
    writer.writeStartElement( dgml::dgmlTag_Head );
    writer.writeElement( "name", head->name() );
    writer.writeElement( "target", head->target() );
    writer.writeElement( "theme", head->theme() );
    writer.writeElement( "visible", head->visible() ? "true" : "false" );
    writer.writeStartElement( "description" );
    writer.writeCDATA( head->description() );
    writer.writeEndElement();
    
    const GeoSceneIcon &icon = static_cast<const GeoSceneIcon&>( *head->icon() );
    writer.writeStartElement( dgml::dgmlTag_Icon );
    writer.writeAttribute( "pixmap", icon.pixmap() );
    writer.writeEndElement();
    
    const GeoSceneZoom &zoom = static_cast<const GeoSceneZoom&>( *head->zoom() );
    writer.writeStartElement( dgml::dgmlTag_Zoom );
    writer.writeElement( "discrete", zoom.discrete() ? "true" : "false" );
    writer.writeTextElement( "minimum", QString::number( zoom.minimum() ) );
    writer.writeTextElement( "maximum", QString::number( zoom.maximum() ) );
    writer.writeEndElement();
    
    writer.writeEndElement();

    return true;
}

}
