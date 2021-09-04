/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlNameTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataFeature.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( name )

GeoNode* KmlnameTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_name)));

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.is<GeoDataFeature>() ) {
        QString name = parser.readElementText().trimmed();
        
        parentItem.nodeAs<GeoDataFeature>()->setName( name );
    }

    return nullptr;
}

}
}
