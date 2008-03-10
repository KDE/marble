/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

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

#include <QDebug>

#include "DGMLPropertyTagHandler.h"

#include "DGMLElementDictionary.h"
#include "DGMLAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneSettings.h"
#include "GeoSceneProperty.h"

using namespace GeoSceneElementDictionary;
using namespace GeoSceneAttributeDictionary;

DGML_DEFINE_TAG_HANDLER(Property)

DGMLPropertyTagHandler::DGMLPropertyTagHandler()
    : GeoTagHandler()
{
    /* NOOP */
}

DGMLPropertyTagHandler::~DGMLPropertyTagHandler()
{
    /* NOOP */
}

GeoNode* DGMLPropertyTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_Property));

    GeoSceneProperty* property = new GeoSceneProperty;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Settings)) {
        parentItem.nodeAs<GeoSceneSettings>()->addProperty(property);
        property->setName(parser.attribute(dgmlAttr_name));
    }

    return property;
}
