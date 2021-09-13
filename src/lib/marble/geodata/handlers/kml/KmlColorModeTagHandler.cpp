/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlColorModeTagHandler.h"

#include "MarbleDebug.h"
#include "KmlElementDictionary.h"
#include "GeoDataColorStyle.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( colorMode )

GeoNode* KmlcolorModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_colorMode)));

    GeoStackItem  parentItem = parser.parentElement();
    
    if ( parentItem.is<GeoDataColorStyle>() ) {
        if (parser.readElementText().trimmed() == QLatin1String("random")) {
            parentItem.nodeAs<GeoDataColorStyle>()->setColorMode( GeoDataColorStyle::Random );
        } else {
            parentItem.nodeAs<GeoDataColorStyle>()->setColorMode( GeoDataColorStyle::Normal );
        }
    }

    return nullptr;
}

}
}
