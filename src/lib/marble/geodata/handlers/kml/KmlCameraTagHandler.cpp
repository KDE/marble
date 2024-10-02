
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//
#include "KmlCameraTagHandler.h"

#include "GeoDataCamera.h"
#include "GeoDataFeature.h"
#include "GeoDataFlyTo.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER(Camera)

GeoNode *KmlCameraTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Camera)));

    GeoDataCamera *camera = nullptr;
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>()) {
        camera = new GeoDataCamera;
        KmlObjectTagHandler::parseIdentifiers(parser, camera);
        parentItem.nodeAs<GeoDataFeature>()->setAbstractView(camera);
    }

    if (parentItem.is<GeoDataFlyTo>()) {
        camera = new GeoDataCamera;
        KmlObjectTagHandler::parseIdentifiers(parser, camera);
        parentItem.nodeAs<GeoDataFlyTo>()->setView(camera);
    }

    return camera;
}

}

}
