//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010          Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlValueTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataData.h"
#include "GeoDataSimpleArrayData.h"
#include "GeoParser.h"

#include <QtCore/QVariant>

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( value )
KML_DEFINE_TAG_HANDLER_GX22( value )

GeoNode* KmlvalueTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_value ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Data ) ) {
        QString value = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataData>()->setValue( QVariant( value ) );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_value << "> containing: " << value
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    } else if( parentItem.represents( kmlTag_SimpleArrayData ) ) {
        QString value = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataSimpleArrayData>()->append( QVariant( value ) );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_SimpleArrayData << "> containing: " << value
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}
