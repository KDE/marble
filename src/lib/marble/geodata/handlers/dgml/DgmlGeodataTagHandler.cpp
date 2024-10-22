/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlGeodataTagHandler.h"

#include <limits>

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneDocument.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneLayer.h"
#include "GeoSceneSettings.h"

namespace Marble
{
namespace dgml
{

DGML_DEFINE_TAG_HANDLER(Geodata)

GeoNode *DgmlGeodataTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Geodata)));

    const QString name = parser.attribute(dgmlAttr_name).trimmed();

    const QString property = parser.attribute(dgmlAttr_property).trimmed();

    const QString colorize = parser.attribute(dgmlAttr_colorize).trimmed();

    const QString expireStr = parser.attribute(dgmlAttr_expire).trimmed();
    int expire = std::numeric_limits<int>::max();
    if (!expireStr.isEmpty())
        expire = expireStr.toInt();

    GeoSceneGeodata *dataSource = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    // Check parent type and make sure that the dataSet type
    // matches the backend of the parent layer
    if (parentItem.represents(dgmlTag_Layer) && parentItem.nodeAs<GeoSceneLayer>()->backend() == QString::fromLatin1(dgmlValue_geodata)) {
        dataSource = new GeoSceneGeodata(name);
        dataSource->setProperty(property);
        dataSource->setColorize(colorize);
        dataSource->setExpire(expire);
        parentItem.nodeAs<GeoSceneLayer>()->addDataset(dataSource);
    }

    return dataSource;
}

}
}
