/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Target)));

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

    return nullptr;
}

}
}
