//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Levente Kurusa  <levex@linux.com>
//

#include "KmlSnippetTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataFeature.h"
#include "GeoDataParser.h"
#include "GeoDataSnippet.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Snippet )

GeoNode* KmlSnippetTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Snippet)));

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.is<GeoDataFeature>() ) {
        QString text = parser.readElementText().trimmed();
        int maxLines = parser.attribute( "maxLines" ).trimmed().toInt();

        parentItem.nodeAs<GeoDataFeature>()->setSnippet( GeoDataSnippet(text, maxLines) );
    }

    return 0;
}

}
}
