/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
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
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_SimpleField ) );

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

    return 0;
}

GeoDataSimpleField::SimpleFieldType KmlSimpleFieldTagHandler::resolveType( const QString& type )
{
    GeoDataSimpleField::SimpleFieldType fieldType;
    if ( type == QString("string") ) {
        fieldType = GeoDataSimpleField::String;
    }
    else if ( type == QString("int") ) {
        fieldType = GeoDataSimpleField::Int;
    }
    else if ( type == QString("unit") ) {
        fieldType = GeoDataSimpleField::UInt;
    }
    else if ( type == QString("short") ) {
        fieldType = GeoDataSimpleField::Short;
    }
    else if ( type == QString("ushort") ) {
        fieldType = GeoDataSimpleField::UShort;
    }
    else if ( type == QString("float") ) {
        fieldType = GeoDataSimpleField::Float;
    }
    else if ( type == QString("double") ) {
        fieldType = GeoDataSimpleField::Double;
    }
    else {
        fieldType = GeoDataSimpleField::Bool;
    }
    return fieldType;
}

}
}
