// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlGxAltitudeModeTagHandler.h"

#include "MarbleDebug.h"

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
namespace gx
{
KML_DEFINE_TAG_HANDLER_GX22(altitudeMode)

GeoNode *KmlaltitudeModeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_altitudeMode)));

    QString content = parser.readElementText().trimmed();

    AltitudeMode mode;
    if (content == QLatin1StringView("relativeToGround")) {
        mode = RelativeToGround;
    } else if (content == QLatin1StringView("absolute")) {
        mode = Absolute;
    } else if (content == QLatin1StringView("relativeToSeaFloor")) {
        mode = RelativeToSeaFloor;
    } else if (content == QLatin1StringView("clampToSeaFloor")) {
        mode = ClampToSeaFloor;
    } else if (content == QLatin1StringView("clampToGround")) {
        mode = ClampToGround;
    } else {
        mDebug() << "Unknown altitude mode " << content << ", falling back to 'clampToGround'";
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
}
