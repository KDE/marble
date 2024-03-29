// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Niko Sams <niko.sams@gmail.com>
//

#include "KmlSimpleArrayDataTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataExtendedData.h"
#include "GeoDataSimpleArrayData.h"
#include "GeoDataSchemaData.h"
#include "GeoDataTypes.h"

#include "GeoParser.h"
#include "GeoDocument.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( SimpleArrayData )

GeoNode* KmlSimpleArrayDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_SimpleArrayData)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataExtendedData>() ) {
        GeoDataSimpleArrayData *arrayData = new GeoDataSimpleArrayData();
        QString name = parser.attribute( "name" ).trimmed();
        parentItem.nodeAs<GeoDataExtendedData>()->setSimpleArrayData( name, arrayData );
        return arrayData;
    }

    if ( parentItem.is<GeoDataSchemaData>() ) {
        GeoDataExtendedData *extendedData = parentItem.nodeAs<GeoDataSchemaData>()->parent();
        GeoDataSimpleArrayData *arrayData = new GeoDataSimpleArrayData;
        const QString name = parser.attribute("name").trimmed();
        extendedData->setSimpleArrayData(name, arrayData);
        return arrayData;
    }

    return nullptr;
}

}
}
