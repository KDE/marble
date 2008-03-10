/*
    Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>

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

#include "DGMLDiscreteTagHandler.h"

#include "DGMLElementDictionary.h"
#include "DGMLAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneZoom.h"

using namespace GeoSceneElementDictionary;
using namespace GeoSceneAuxillaryDictionary;

DGML_DEFINE_TAG_HANDLER(Discrete)

DGMLDiscreteTagHandler::DGMLDiscreteTagHandler()
    : GeoTagHandler()
{
}

DGMLDiscreteTagHandler::~DGMLDiscreteTagHandler()
{
}

GeoNode* DGMLDiscreteTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_Discrete));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Zoom))
    {
        bool discrete = false;
        QString parsedText = parser.readElementText().toLower().trimmed();
        if (   parsedText == dgmlValue_true || parsedText == dgmlValue_on )
        {
            discrete = true;
        }
        parentItem.nodeAs<GeoSceneZoom>()->setDiscrete( discrete );
    }

    return 0;
}
