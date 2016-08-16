//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlMaxWidthTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataImagePyramid.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( maxWidth )

GeoNode* KmlmaxWidthTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_maxWidth)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ImagePyramid ))
    {
        int maxWidth = parser.readElementText().toInt();

        parentItem.nodeAs<GeoDataImagePyramid>()->setMaxWidth( maxWidth );
    }
    return 0;
}

}
}
