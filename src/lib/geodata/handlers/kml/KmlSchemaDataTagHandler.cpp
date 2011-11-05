/*
    Copyright (C) 2011 Niko Sams <niko.sams@gmail.com>

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

#include "KmlSchemaDataTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataExtendedData.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( SchemaData )

GeoNode* KmlSchemaDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_SchemaData ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_ExtendedData ) ) {
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_SchemaData << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS

        QString schemaUrl = parser.attribute( "schemaUrl" ).trimmed();
#ifdef DEBUG_TAGS
        mDebug() << "attribute 'schemaUrl':" << schemaUrl;
#endif // DEBUG_TAGS

        return parentItem.nodeAs<GeoDataExtendedData>();
    } else {
        return 0;
    }

}

}
}
