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

    if (!map->center().isEmpty()) {
        if (map->center().count() == 2) {
            writer.writeElement(dgml::dgmlTag_Center,
                                  map->center().at(0).toString() + ","
                                + map->center().at(1).toString());
        }
        else if (map->center().count() == 4) {
            writer.writeElement(dgml::dgmlTag_Center,
                                  map->center().at(0).toString() + ","
                                + map->center().at(1).toString() + ","
                                + map->center().at(2).toString() + ","
                                + map->center().at(3).toString());
        }
    }
    
    writer.writeStartElement( dgml::dgmlTag_Target);
    writer.writeEndElement();
    
    for( int i = 0; i < map->layers().count(); ++i )
    {
        writeElement( map->layers().at( i ), writer );
    }
    
    writer.writeEndElement();
    return true;
}

}
