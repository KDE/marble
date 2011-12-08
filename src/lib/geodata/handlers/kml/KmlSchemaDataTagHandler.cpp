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

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( SchemaData )

GeoNode* KmlSchemaDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_SchemaData ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_ExtendedData ) ) {
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_SchemaData << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS

        QString schemaUrl = parser.attribute( "schemaUrl" ).trimmed();
#ifdef DEBUG_TAGS
        mDebug() << "attribute 'schemaUrl':" << schemaUrl;
#endif // DEBUG_TAGS

        return parentItem.nodeAs<GeoDataExtendedData>();
    } else {
        return 0;
    }

}

}
}
