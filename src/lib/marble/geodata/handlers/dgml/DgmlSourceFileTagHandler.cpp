/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlSourceFileTagHandler.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneGeodata.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(SourceFile)

GeoNode* DgmlSourceFileTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_SourceFile)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.represents( dgmlTag_Vector )
         || parentItem.represents( dgmlTag_Geodata ) ) {
        GeoSceneGeodata *dataSource = nullptr;
        dataSource = parentItem.nodeAs<GeoSceneGeodata>();
        dataSource->setSourceFile( parser.readElementText().trimmed() );
    }

    return nullptr;
}

}
}
