/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlFolderTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataContainer.h"
#include "GeoDataFolder.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Folder)

GeoNode* KmlFolderTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Folder)));

    GeoStackItem parentItem = parser.parentElement();
    GeoDataFolder *folder = new GeoDataFolder;
    KmlObjectTagHandler::parseIdentifiers( parser, folder );
    if ( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ) {
        GeoDataContainer *parentPtr = parentItem.nodeAs<GeoDataContainer>();
        parentPtr->append( folder );

        return folder;
    } else if (parentItem.qualifiedName().first == QLatin1String(kmlTag_kml)) {
        GeoDataDocument* doc = geoDataDoc( parser );
        doc->append( folder );
        return folder;
    } else {
        delete folder;
        return nullptr;
    }
}

}
}
