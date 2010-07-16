/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "KmlDocumentTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Document)

GeoNode* KmlDocumentTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(kmlTag_Document));

    GeoStackItem parentItem = parser.parentElement();
    if( !(parentItem.first.first.isNull() && parentItem.first.second.isNull()) ) {
        // this happens if there is a parent element to the Document tag. We can work around that and simply expect that
        // the new Document tag works like a Folder
        if( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ) {
            GeoDataDocument *document = new GeoDataDocument;
            parentItem.nodeAs<GeoDataContainer>()->append( document );

#ifdef DEBUG_TAGS
            mDebug() << "Parsed <" << kmlTag_Document << "> containing: " << &parentItem.nodeAs<GeoDataContainer>()->last()
                     << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
            return document;
        }
        else if ( parentItem.first.first == kmlTag_kml)
        {
            GeoDataDocument* doc = geoDataDoc( parser );
#ifdef DEBUG_TAGS
            mDebug() << "Parsed <" << kmlTag_Document << "> document: " << doc;
#endif
            return doc;
        }
    }
    return 0;
}

}
}
