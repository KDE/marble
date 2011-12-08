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

#include "KmlSimpleArrayDataTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataExtendedData.h"
#include "GeoDataSimpleArrayData.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( SimpleArrayData )

GeoNode* KmlSimpleArrayDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_SimpleArrayData ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataExtendedData>() ) {
        GeoDataSimpleArrayData *arrayData = new GeoDataSimpleArrayData();
        QString name = parser.attribute( "name" ).trimmed();
        parentItem.nodeAs<GeoDataExtendedData>()->setSimpleArrayData( name, arrayData );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_SimpleArrayData << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        return arrayData;
    }

    return 0;
}

}
}
