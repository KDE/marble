/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlValueTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneProperty.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Value)

GeoNode* DgmlValueTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Value)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Property)) {
        QString parsedText = parser.readElementText().toLower().trimmed();
        parentItem.nodeAs<GeoSceneProperty>()->setDefaultValue(parsedText == dgmlValue_true || parsedText == dgmlValue_on);
    }

    return nullptr;
}

}
}
