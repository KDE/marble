/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlPlacemarkTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataContainer.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Placemark )

GeoNode* KmlPlacemarkTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Placemark)));

    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    KmlObjectTagHandler::parseIdentifiers( parser, placemark );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ||
        parentItem.represents( kmlTag_Change ) || parentItem.represents( kmlTag_Create ) || parentItem.represents( kmlTag_Delete ) ){
        parentItem.nodeAs<GeoDataContainer>()->append( placemark );
        return placemark;
    } else if ( parentItem.qualifiedName().first == kmlTag_kml ) {
        GeoDataDocument* doc = geoDataDoc(parser);
        doc->append( placemark );
        return placemark;
    } else {
        delete placemark;
        return nullptr;
    }
}

}
}
