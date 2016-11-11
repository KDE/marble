//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>

#include "KmlLinkTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataLink.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataModel.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Link )

GeoNode* KmlLinkTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Link)));
    GeoDataLink link;
    KmlObjectTagHandler::parseIdentifiers( parser, &link );
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_NetworkLink )) {
        return &parentItem.nodeAs<GeoDataNetworkLink>()->link();
    } else if( parentItem.represents( kmlTag_Model ) ) {
        parentItem.nodeAs<GeoDataModel>()->setLink(link);
        return &parentItem.nodeAs<GeoDataModel>()->link();
    }else{
        return 0;
    }
}

}
}
