/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifdef KML_LAZY_IMP
#include "KmlMarblePlacemarkTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataContainer.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( MarblePlacemark )

GeoNode* KmlMarblePlacemarkTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_MarblePlacemark)));


    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ) {
        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        parentItem.nodeAs<GeoDataContainer>()->append( placemark );
        return placemark;

    } else {
        return nullptr;
    }
}

}
}

#endif
