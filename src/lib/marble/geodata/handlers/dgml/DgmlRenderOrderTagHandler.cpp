// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Sanjiban Bairagya <sanjibanb@kde.org>
//

#include "DgmlRenderOrderTagHandler.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneGeodata.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(RenderOrder)

GeoNode* DgmlRenderOrderTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_RenderOrder)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.represents( dgmlTag_Vector )
         || parentItem.represents( dgmlTag_Geodata ) ) {
        GeoSceneGeodata *dataSource = nullptr;
        dataSource = parentItem.nodeAs<GeoSceneGeodata>();
        dataSource->setRenderOrder( parser.readElementText().trimmed().toInt() );
    }

    return nullptr;
}

}
}
