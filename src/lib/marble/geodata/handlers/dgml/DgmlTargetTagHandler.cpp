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

#include "DgmlTargetTagHandler.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Target)

GeoNode* DgmlTargetTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_Target));

    QString const targetRadius = parser.attribute( dgmlAttr_radius ).trimmed();

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Head)) {
        parentItem.nodeAs<GeoSceneHead>()->setTarget( parser.readElementText().trimmed() );
        if ( !targetRadius.isEmpty() ) {
            parentItem.nodeAs<GeoSceneHead>()->setRadius( targetRadius.toDouble() );
        }
    }

    if (parentItem.represents(dgmlTag_Map)) {
/*        GeoSceneLayer *layer = new GeoSceneLayer( "$MARBLETARGET$" );
        parentItem.nodeAs<GeoSceneMap>()->addLayer( layer );*/
    }

    return 0;
}

}
}
