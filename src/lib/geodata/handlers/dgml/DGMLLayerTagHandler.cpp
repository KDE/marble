/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2008 Torsten Rahn <tackat@kde.org>

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

#include "DGMLLayerTagHandler.h"

#include <QDebug>

#include "DGMLElementDictionary.h"
#include "DGMLAttributeDictionary.h"
#include "DGMLAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneMap.h"
#include "GeoSceneLayer.h"

using namespace GeoSceneElementDictionary;
using namespace GeoSceneAttributeDictionary;
using namespace GeoSceneAuxillaryDictionary;

DGML_DEFINE_TAG_HANDLER(Layer)

DGMLLayerTagHandler::DGMLLayerTagHandler()
    : GeoTagHandler()
{
}

DGMLLayerTagHandler::~DGMLLayerTagHandler()
{
}

GeoNode* DGMLLayerTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_Layer));

    QString name      = parser.attribute(dgmlAttr_name);
    QString plugin    = parser.attribute(dgmlAttr_plugin);

    GeoSceneLayer *layer = 0;

    // Checking for parent layer
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Map)) {
        layer = new GeoSceneLayer( name );
        layer->setPlugin( plugin );
        parentItem.nodeAs<GeoSceneMap>()->addLayer( layer );
    }

    return layer;
}
