/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlVectorTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneLayer.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Vector)

GeoNode *DgmlVectorTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Vector)));

    QString name = parser.attribute(dgmlAttr_name).trimmed();
    QString feature = parser.attribute(dgmlAttr_feature).trimmed();

    GeoSceneGeodata *vector = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    // Check parent type and make sure that the dataSet type
    // matches the backend of the parent layer
    if (parentItem.represents(dgmlTag_Layer) && parentItem.nodeAs<GeoSceneLayer>()->backend() == QString::fromLatin1(dgmlValue_vector)) {
        vector = new GeoSceneGeodata(name);
        vector->setProperty(feature);
        vector->setColorize(feature);
        parentItem.nodeAs<GeoSceneLayer>()->addDataset(vector);
    }

    return vector;
}

}
}
