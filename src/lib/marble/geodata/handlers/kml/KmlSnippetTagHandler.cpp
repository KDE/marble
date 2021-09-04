// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Levente Kurusa <levex@linux.com>
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

    return nullptr;
}

}
}
