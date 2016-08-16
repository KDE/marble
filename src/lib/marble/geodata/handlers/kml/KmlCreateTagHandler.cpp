//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlCreateTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataUpdate.h"
#include "GeoDataCreate.h"
#include "GeoDataParser.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Create )

GeoNode* KmlCreateTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Create)));

    GeoDataCreate *create = new GeoDataCreate;
    KmlObjectTagHandler::parseIdentifiers( parser, create );
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Update ) ) {
        parentItem.nodeAs<GeoDataUpdate>()->setCreate( create );
        return create;
    } else {
        delete create;
        return 0;
    }
}

}
}
