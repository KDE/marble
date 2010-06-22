/*
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

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

// Own
#include "DgmlProjectionTagHandler.h"

// Marble
#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTexture.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Projection)

GeoNode* DgmlProjectionTagHandler::parse( GeoParser& parser ) const
{
    // Check whether the tag is valid
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( dgmlTag_Projection ));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( !parentItem.represents( dgmlTag_Texture ))
        return 0;

    // Attribute name, default to "Equirectangular"
    const QString nameStr = parser.attribute( dgmlAttr_name ).trimmed();
    if ( !nameStr.isEmpty() ) {
        GeoSceneTexture::Projection projection = GeoSceneTexture::Equirectangular;
        if ( nameStr == "Equirectangular" )
            projection = GeoSceneTexture::Equirectangular;
        else if ( nameStr == "Mercator" )
            projection = GeoSceneTexture::Mercator;
        else
            parser.raiseWarning( QString( "Value not allowed for attribute name: %1" ).arg( nameStr ));

        parentItem.nodeAs<GeoSceneTexture>()->setProjection( projection );
    }
    return 0;
}

}
}
