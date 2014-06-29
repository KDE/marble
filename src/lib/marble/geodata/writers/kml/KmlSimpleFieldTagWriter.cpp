//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include <QString>

#include "KmlSimpleFieldTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSimpleField(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataSimpleFieldType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlSimpleFieldTagWriter );

bool KmlSimpleFieldTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataSimpleField *simpleField = static_cast<const GeoDataSimpleField*>( node );
    writer.writeStartElement( kml::kmlTag_SimpleField );
    writer.writeAttribute( "name", simpleField->name() );
    GeoDataSimpleField::SimpleFieldType simpleFieldType = simpleField->type();
    QString type = resolveType( simpleFieldType );
    writer.writeAttribute( "type", type );

    QString displayName = simpleField->displayName();
    writer.writeElement( "displayName", displayName );

    writer.writeEndElement();

    return true;
}

QString KmlSimpleFieldTagWriter::resolveType( GeoDataSimpleField::SimpleFieldType type )
{
    switch (type)
    {
    case GeoDataSimpleField::String: return "string";
    case GeoDataSimpleField::Int:    return "int";
    case GeoDataSimpleField::UInt:   return "uint";
    case GeoDataSimpleField::Short:  return "short";
    case GeoDataSimpleField::UShort: return "ushort";
    case GeoDataSimpleField::Float:  return "float";
    case GeoDataSimpleField::Double: return "double";
    case GeoDataSimpleField::Bool:   return "bool";
    }

    Q_ASSERT(false);
    return "string";
}

}
