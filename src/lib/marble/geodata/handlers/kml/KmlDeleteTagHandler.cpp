//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlDeleteTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataUpdate.h"
#include "GeoDataDelete.h"
#include "GeoDataParser.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Delete )

GeoNode* KmlDeleteTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Delete)));

    GeoDataDelete *dataDelete = new GeoDataDelete;
    KmlObjectTagHandler::parseIdentifiers( parser, dataDelete );
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Update ) ) {
        parentItem.nodeAs<GeoDataUpdate>()->setDelete( dataDelete );
        return dataDelete;
    } else {
        delete dataDelete;
        return 0;
    }
}

}
}
