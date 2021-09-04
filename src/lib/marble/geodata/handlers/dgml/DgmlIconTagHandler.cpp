/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "DgmlIconTagHandler.h"

// Qt
#include <QColor>

// Marble
#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneItem.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Icon)

GeoNode* DgmlIconTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Icon)));

    QString pixmapRelativePath  = parser.attribute(dgmlAttr_pixmap).trimmed();

    QColor color;
    color.setNamedColor(parser.attribute(dgmlAttr_color).trimmed());

    GeoSceneIcon *icon = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Head)) {
        icon = parentItem.nodeAs<GeoSceneHead>()->icon();
        icon->setPixmap( pixmapRelativePath );
        icon->setColor( color );
    }
    if (parentItem.represents(dgmlTag_Item)) {
        icon = parentItem.nodeAs<GeoSceneItem>()->icon();
        icon->setPixmap( pixmapRelativePath );
        icon->setColor( color );
    }

    return nullptr;
}

}
}
