// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlShapeTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataParser.h"
#include "GeoDataPhotoOverlay.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(shape)

GeoNode *KmlshapeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_shape)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_PhotoOverlay)) {
        GeoDataPhotoOverlay::Shape shape;
        QString shapeText = parser.readElementText();

        if (shapeText == QLatin1String("rectangle")) {
            shape = GeoDataPhotoOverlay::Rectangle;
        } else if (shapeText == QLatin1String("cylinder")) {
            shape = GeoDataPhotoOverlay::Cylinder;
        } else if (shapeText == QLatin1String("sphere")) {
            shape = GeoDataPhotoOverlay::Sphere;
        } else {
            mDebug() << "Unknown shape attribute" << shapeText << ", falling back to default value 'rectangle'";
            shape = GeoDataPhotoOverlay::Rectangle;
        }

        parentItem.nodeAs<GeoDataPhotoOverlay>()->setShape(shape);
    }
    return nullptr;
}

}
}
