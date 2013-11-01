//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlGridOriginTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataImagePyramid.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( gridOrigin )

GeoNode* KmlgridOriginTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_gridOrigin ) );

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ImagePyramid ))
    {
        GeoDataImagePyramid::GridOrigin gridOrigin;
        QString gridOriginText = parser.readElementText();

        if ( gridOriginText == "lowerLeft" ) {
            gridOrigin = GeoDataImagePyramid::LowerLeft;
        } else if ( gridOriginText == "upperLeft" ) {
            gridOrigin = GeoDataImagePyramid::UpperLeft;
        }

        parentItem.nodeAs<GeoDataImagePyramid>()->setGridOrigin( gridOrigin );
    }
    return 0;
}

}
}
