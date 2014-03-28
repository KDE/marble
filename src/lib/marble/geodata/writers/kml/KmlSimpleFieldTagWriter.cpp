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
                                 kml::kmlTag_nameSpace22 ),
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

QString KmlSimpleFieldTagWriter::resolveType( GeoDataSimpleField::SimpleFieldType& type ) const
{
    QString simpleFieldType;
    if ( type == GeoDataSimpleField::String ) {
        simpleFieldType = QString("string");
    }
    else if ( type == GeoDataSimpleField::Int ) {
        simpleFieldType = QString("int");
    }
    else if ( type == GeoDataSimpleField::UInt ) {
        simpleFieldType = QString("uint");
    }
    else if (type == GeoDataSimpleField::Short ) {
        simpleFieldType = QString("short");
    }
    else if ( type == GeoDataSimpleField::UShort ) {
        simpleFieldType = QString("ushort");
    }
    else if ( type == GeoDataSimpleField::Float ) {
        simpleFieldType = QString("float");
    }
    else if ( type == GeoDataSimpleField::Double ) {
        simpleFieldType = QString("double");
    }
    else {
        simpleFieldType = QString("bool");
    }
    return simpleFieldType;
}

}
