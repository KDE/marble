//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "KmlSchemaDataTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataExtendedData.h"
#include "GeoDataSchemaData.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( SchemaData )

GeoNode* KmlSchemaDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_SchemaData)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_ExtendedData ) ) {

        GeoDataSchemaData schemaData;
        QString schemaUrl = parser.attribute( "schemaUrl" ).trimmed();
        schemaData.setSchemaUrl( schemaUrl );
        parentItem.nodeAs<GeoDataExtendedData>()->addSchemaData( schemaData );
        return &parentItem.nodeAs<GeoDataExtendedData>()->schemaData( schemaUrl );
    }
    return 0;
}

} //namespace kml

} // namespace Marble
