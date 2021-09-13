/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlSimpleFieldTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataSchema.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( SimpleField )

GeoNode* KmlSimpleFieldTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_SimpleField)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Schema ) ) {
        GeoDataSimpleField simpleField;
        QString name = parser.attribute( "name" ).trimmed();
        QString type = parser.attribute( "type" ).trimmed();
        simpleField.setName( name );
        GeoDataSimpleField::SimpleFieldType fieldType = resolveType( type );
        simpleField.setType( fieldType );
        parentItem.nodeAs<GeoDataSchema>()->addSimpleField( simpleField );
        return &parentItem.nodeAs<GeoDataSchema>()->simpleField( name );
    }

    return nullptr;
}

GeoDataSimpleField::SimpleFieldType KmlSimpleFieldTagHandler::resolveType( const QString& type )
{
    GeoDataSimpleField::SimpleFieldType fieldType;
    if (type == QLatin1String("string")) {
        fieldType = GeoDataSimpleField::String;
    }
    else if (type == QLatin1String("int")) {
        fieldType = GeoDataSimpleField::Int;
    }
    else if (type == QLatin1String("unit")) {
        fieldType = GeoDataSimpleField::UInt;
    }
    else if (type == QLatin1String("short")) {
        fieldType = GeoDataSimpleField::Short;
    }
    else if (type == QLatin1String("ushort")) {
        fieldType = GeoDataSimpleField::UShort;
    }
    else if (type == QLatin1String("float")) {
        fieldType = GeoDataSimpleField::Float;
    }
    else if (type == QLatin1String("double")) {
        fieldType = GeoDataSimpleField::Double;
    }
    else {
        fieldType = GeoDataSimpleField::Bool;
    }
    return fieldType;
}

}
}
