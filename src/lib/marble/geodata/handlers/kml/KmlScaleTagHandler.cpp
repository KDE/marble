//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlScaleTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataScale.h"
#include "GeoDataGeometry.h"
#include "GeoDataModel.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Scale )

GeoNode* KmlScaleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Scale ) );

    GeoDataScale scale;
    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Model ) ) {
        parentItem.nodeAs<GeoDataModel>()->setScale(scale);
        return &parentItem.nodeAs<GeoDataModel>()->scale();
    } else {
        return 0;
    }
}

}
}
