//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlGeodataTagWriter.h"

#include "GeoWriter.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneTypes.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerGeodata( GeoTagWriter::QualifiedName( GeoSceneTypes::GeoSceneGeodataType, dgml::dgmlTag_nameSpace20 ), new DgmlGeodataTagWriter() );

bool DgmlGeodataTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoSceneGeodata *geodata = static_cast<const GeoSceneGeodata*>( node );
    writer.writeStartElement( dgml::dgmlTag_Geodata );
    writer.writeAttribute( "name", geodata->name() );
    
    writer.writeStartElement( dgml::dgmlTag_SourceFile );
    writer.writeCharacters( geodata->sourceFile() );
    writer.writeEndElement();
    
    writer.writeEndElement();
    return true;
}

}
