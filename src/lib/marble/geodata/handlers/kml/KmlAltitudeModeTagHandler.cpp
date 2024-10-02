/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
    SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlAltitudeModeTagHandler.h"

#include "KmlElementDictionary.h"

#include "GeoDataAbstractView.h"
#include "GeoDataGeometry.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataModel.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataTrack.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(altitudeMode)

GeoNode *KmlaltitudeModeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_altitudeMode)));

    QString content = parser.readElementText().trimmed();

    AltitudeMode mode;
    if (content == QLatin1StringView("relativeToGround")) {
        mode = RelativeToGround;
    } else if (content == QLatin1StringView("absolute")) {
        mode = Absolute;
    } else { // clampToGround is Standard
        mode = ClampToGround;
    }

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataPlacemark>()) {
        parentItem.nodeAs<GeoDataPlacemark>()->geometry()->setAltitudeMode(mode);
    } else if (parentItem.is<GeoDataPoint>()) {
        parentItem.nodeAs<GeoDataPoint>()->setAltitudeMode(mode);
    } else if (parentItem.is<GeoDataLatLonAltBox>()) {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setAltitudeMode(mode);
    } else if (parentItem.is<GeoDataTrack>()) {
        parentItem.nodeAs<GeoDataTrack>()->setAltitudeMode(mode);
    } else if (parentItem.is<GeoDataGroundOverlay>()) {
        parentItem.nodeAs<GeoDataGroundOverlay>()->setAltitudeMode(mode);
    } else if (parentItem.is<GeoDataAbstractView>()) {
        parentItem.nodeAs<GeoDataAbstractView>()->setAltitudeMode(mode);
    } else if (parentItem.is<GeoDataModel>()) {
        parentItem.nodeAs<GeoDataModel>()->setAltitudeMode(mode);
    }

    return nullptr;
}

}
}
