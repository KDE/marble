//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlMxSizeTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataIconStyle.h"
#include "GeoParser.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace kml
{
namespace mx
{
KML_DEFINE_TAG_HANDLER_MX( size )

GeoNode* KmlsizeTagHandler::parse( GeoParser& parser ) const
{
    if ( parser.parentElement().is<GeoDataIconStyle>() ) {
        auto const width = parser.attribute(kmlTag_width).toInt();
        auto const height = parser.attribute(kmlTag_height).toInt();
        auto const size = QSize(width, height);
        if (size.isEmpty()) {
            mDebug() << "Invalid size in icon style: width=" << width << ", height=" << height;
        } else {
            parser.parentElement().nodeAs<GeoDataIconStyle>()->setSize(size);
        }
    }

    return 0;
}

}
}
}
