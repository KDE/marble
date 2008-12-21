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

#include <QtCore/QDebug>

#include "KmlElementDictionary.h"

#include "GeoDataParser.h"

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
#ifdef DEBUG_TAGS
        qDebug() << "Parsed <" << kmlTag_SimpleField << "> "
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        QString name = parser.attribute( "name" ).trimmed();
        QString type = parser.attribute( "type" ).trimmed();
#ifdef DEBUG_TAGS
        qDebug() << "adding new Tag" << name << "of type" << type << "to parent Tag.";
        qDebug() << "not implemented yet.";
#endif // DEBUG_TAGS
    }

    return 0;
}

}
}
