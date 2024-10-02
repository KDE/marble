/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// be aware that there is another Tag called Scale which uses the name KmlscaleTagHandler.h
// as it is impossible to use the name KmlscaleTagHandler then, use an underscore
// to mark the lower case variant

#include "Kml_scaleTagHandler.h"

#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(scale)

GeoNode *KmlscaleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_scale)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_IconStyle)) {
        parentItem.nodeAs<GeoDataIconStyle>()->setScale(parser.readElementText().trimmed().toFloat());
    } else if (parentItem.represents(kmlTag_LabelStyle)) {
        parentItem.nodeAs<GeoDataLabelStyle>()->setScale(parser.readElementText().trimmed().toFloat());
    }
    return nullptr;
}

}
}
