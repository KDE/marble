/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlMaximumTagHandler.h"

#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneZoom.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Maximum)

GeoNode *DgmlMaximumTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Maximum)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Zoom)) {
        QString parsedText = parser.readElementText();

        bool ok = false;
        int parsedInt = parsedText.toInt(&ok);

        if (ok)
            parentItem.nodeAs<GeoSceneZoom>()->setMaximum(parsedInt);
        else
            parser.raiseWarning(QObject::tr("Could not convert <maximum> child text content to integer. Was: '%1'").arg(parsedText));
    }

    return nullptr;
}

}
}
