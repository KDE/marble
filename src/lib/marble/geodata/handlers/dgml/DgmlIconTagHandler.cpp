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
    if (pixmapRelativePath.isEmpty()) {
        color.setNamedColor(parser.attribute(dgmlAttr_color).trimmed());
    }

    GeoSceneIcon *icon = 0;

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

    return 0;
}

}
}
