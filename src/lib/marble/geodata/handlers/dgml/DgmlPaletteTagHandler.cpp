/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2008 Torsten Rahn <tackat@kde.org>
    Copyright (C) 2008 Henry de Valence <hdevalence@gmail.com>

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

#include "DgmlPaletteTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneFilter.h"
#include "GeoScenePalette.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Palette)

GeoNode* DgmlPaletteTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Palette)));

    QString type      = parser.attribute(dgmlAttr_type).toLower().trimmed();
    QString file      = parser.readElementText().trimmed();

    GeoScenePalette *palette = 0;

    // Checking for parent palette
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Filter)) {
        palette = new GeoScenePalette( type, file );
        parentItem.nodeAs<GeoSceneFilter>()->addPalette( palette );
    }

    return palette;
}

}
}
