/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlDiscreteTagHandler.h"

#include "DgmlElementDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneZoom.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Discrete)

GeoNode* DgmlDiscreteTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Discrete)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Zoom)) {
        QString parsedText = parser.readElementText().toLower().trimmed();
        parentItem.nodeAs<GeoSceneZoom>()->setDiscrete(parsedText == dgmlValue_true || parsedText == dgmlValue_on);
    }

    return nullptr;
}

}
}
