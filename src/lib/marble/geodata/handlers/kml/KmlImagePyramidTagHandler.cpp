//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlImagePyramidTagHandler.h"

#include "GeoDataPhotoOverlay.h"
#include "GeoDataImagePyramid.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( ImagePyramid )

GeoNode* KmlImagePyramidTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_ImagePyramid)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_PhotoOverlay ) ) {
        return &parentItem.nodeAs<GeoDataPhotoOverlay>()->imagePyramid();
    }
    return 0;
}

}
}
