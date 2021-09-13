/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlHeadingTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataIconStyle.h"
#include "GeoDataCamera.h"
#include "GeoDataOrientation.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( heading )

GeoNode* KmlheadingTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_heading)));

    GeoStackItem parentItem = parser.parentElement();
    
    int const heading = parser.readElementText().toInt();
    if ( heading >= 0 && heading <= 360 ) {
        if ( parentItem.represents( kmlTag_IconStyle ) ) {
            parentItem.nodeAs<GeoDataIconStyle>()->setHeading( heading );
        } else if ( parentItem.represents( kmlTag_Camera ) ) {
            parentItem.nodeAs<GeoDataCamera>()->setHeading( heading );
        } else if ( parentItem.represents( kmlTag_Orientation ) ) {
	    parentItem.nodeAs<GeoDataOrientation>()->setHeading( heading );
	}
    } else {
        mDebug() << "Invalid heading value " << heading << ", must be within 0..360. Using 0 instead.";
    }

    return nullptr;
}

}
}
