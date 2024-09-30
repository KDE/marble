/*
    SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlStyleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataStyle.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Style)

GeoNode *KmlStyleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Style)));

    GeoStackItem parentItem = parser.parentElement();
    /// for documents several styles are allowed: document wide styles are saved different!!!!!
    if (parentItem.represents(kmlTag_Document)) {
        GeoDataStyle::Ptr style(new GeoDataStyle);
        KmlObjectTagHandler::parseIdentifiers(parser, style.data());
        parentItem.nodeAs<GeoDataDocument>()->addStyle(style);
        return parentItem.nodeAs<GeoDataDocument>()->style(style->id()).data();
    } else if (parentItem.represents(kmlTag_Placemark)) {
        GeoDataStyle::Ptr style(new GeoDataStyle);
        KmlObjectTagHandler::parseIdentifiers(parser, style.data());
        parentItem.nodeAs<GeoDataFeature>()->setStyle(style);
        return style.data();
    }
    // FIXME: KMLStyle can be contained in MultiGeometry as well
    return nullptr;
}

}
}
