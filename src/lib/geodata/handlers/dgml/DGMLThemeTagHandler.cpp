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

#include "DGMLThemeTagHandler.h"

#include "DGMLElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"

using namespace GeoSceneElementDictionary;

DGML_DEFINE_TAG_HANDLER(Theme)

DGMLThemeTagHandler::DGMLThemeTagHandler()
    : GeoTagHandler()
{
}

DGMLThemeTagHandler::~DGMLThemeTagHandler()
{
}

GeoNode* DGMLThemeTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_Theme));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Head))
        parentItem.nodeAs<GeoSceneHead>()->setTheme(parser.readElementText());

    return 0;
}
