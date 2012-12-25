/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "DgmlGeodataTagHandler.h"

#include <limits>

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneDocument.h"
#include "GeoSceneSettings.h"
#include "GeoSceneGeodata.h"

namespace Marble
{
namespace dgml
{

DGML_DEFINE_TAG_HANDLER(Geodata)

GeoNode* DgmlGeodataTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_Geodata));

    const QString name = parser.attribute(dgmlAttr_name).trimmed();

    const QString expireStr = parser.attribute(dgmlAttr_expire).trimmed();
    int expire = std::numeric_limits<int>::max();
    if ( !expireStr.isEmpty() )
        expire = expireStr.toInt();

    GeoSceneGeodata *dataSource = 0;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    // Check parent type and make sure that the dataSet type 
    // matches the backend of the parent layer
    if (parentItem.represents(dgmlTag_Layer)
    && parentItem.nodeAs<GeoSceneLayer>()->backend() == dgmlValue_geodata) {
        dataSource = new GeoSceneGeodata( name );
        dataSource->setExpire( expire );
        parentItem.nodeAs<GeoSceneLayer>()->addDataset( dataSource );
    }

    return dataSource;
}

}
}
