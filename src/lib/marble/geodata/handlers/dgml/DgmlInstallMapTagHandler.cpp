/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlInstallMapTagHandler.h"

#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(InstallMap)

GeoNode* DgmlInstallMapTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_InstallMap)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Texture) || parentItem.represents(dgmlTag_Vectortile))
        parentItem.nodeAs<GeoSceneTileDataset>()->setInstallMap( parser.readElementText().trimmed() );

    return nullptr;
}

}
}
