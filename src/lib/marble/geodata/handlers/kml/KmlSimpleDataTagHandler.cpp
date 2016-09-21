//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "KmlSimpleDataTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataSchemaData.h"
#include "GeoDataSimpleData.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( SimpleData )

GeoNode* KmlSimpleDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_SimpleData)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_SchemaData ) ) {
        GeoDataSimpleData simpleData;
        QString name = parser.attribute( "name" ).trimmed();
        QString data = parser.readElementText().trimmed();
        simpleData.setName( name );
        simpleData.setData( data );
        parentItem.nodeAs<GeoDataSchemaData>()->addSimpleData( simpleData );
    }
    return 0;
}

}
}
