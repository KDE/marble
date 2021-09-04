// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlLinkSnippetTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( linkSnippet )

GeoNode* KmllinkSnippetTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_linkSnippet)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_NetworkLinkControl ) )
    {
        int maxLines = parser.attribute( "maxLines" ).trimmed().toInt();
        QString linkSnippet = parser.readElementText();

        parentItem.nodeAs<GeoDataNetworkLinkControl>()->setLinkSnippet( linkSnippet );
        parentItem.nodeAs<GeoDataNetworkLinkControl>()->setMaxLines( maxLines );
    }

    return nullptr;
}

}
}
