//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlSettingsTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneSettings.h"
#include "GeoSceneProperty.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSettings( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneSettingsType, dgml::dgmlTag_nameSpace20 ),
                                                                            new DgmlSettingsTagWriter() );

bool DgmlSettingsTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneSettings *settings = static_cast<const GeoSceneSettings*>( node );
    writer.writeStartElement( dgml::dgmlTag_Settings );
    
    for( int i = 0; i < settings->allProperties().count(); ++i )
    {
        GeoSceneProperty *property = settings->allProperties().at( i );
        writer.writeStartElement( dgml::dgmlTag_Property );
        writer.writeAttribute( "name", property->name()  );
        writer.writeElement( dgml::dgmlTag_Value, property->value() ? "true" : "false" );
        writer.writeElement( dgml::dgmlTag_Available, property->value() ? "true" : "false" );
        writer.writeEndElement();
    }
    
    writer.writeEndElement();
    return true;
}

};