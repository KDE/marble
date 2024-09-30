/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
    SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlPaletteTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneFilter.h"
#include "GeoScenePalette.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Palette)

GeoNode *DgmlPaletteTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Palette)));

    QString type = parser.attribute(dgmlAttr_type).toLower().trimmed();
    QString file = parser.readElementText().trimmed();

    GeoScenePalette *palette = nullptr;

    // Checking for parent palette
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Filter)) {
        palette = new GeoScenePalette(type, file);
        parentItem.nodeAs<GeoSceneFilter>()->addPalette(palette);
    }

    return palette;
}

}
}
