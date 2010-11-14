//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#include "KmlMaxFadeExtentTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataLod.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( maxFadeExtent )

GeoNode* KmlmaxFadeExtentTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_maxFadeExtent ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Lod ) ) {
        float maxFadeExtent = parser.readElementText().trimmed().toFloat();

        parentItem.nodeAs<GeoDataLod>()->setMaxFadeExtent( maxFadeExtent );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_maxFadeExtent << "> containing: " << maxFadeExtent
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }

    return 0;
}

}
}
