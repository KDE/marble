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

#include <QVariant>

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( value )
KML_DEFINE_TAG_HANDLER_GX22( value )

GeoNode* KmlvalueTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_value)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Data ) ) {
        QString value = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataData>()->setValue( QVariant( value ) );
    } else if( parentItem.represents( kmlTag_SimpleArrayData ) ) {
        QString value = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataSimpleArrayData>()->append( QVariant( value ) );
    }
    return 0;
}

}
}
