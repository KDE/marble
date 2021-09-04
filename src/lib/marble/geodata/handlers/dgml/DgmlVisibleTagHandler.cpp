/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlVisibleTagHandler.h"

#include "DgmlElementDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Visible)

GeoNode* DgmlVisibleTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Visible)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.represents( dgmlTag_Head ) ) {
        QString parsedText = parser.readElementText().toLower().trimmed();
        parentItem.nodeAs<GeoSceneHead>()->setVisible(parsedText == dgmlValue_true || parsedText == dgmlValue_on);
    }

    return nullptr;
}

}
}
