/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlHrefTagHandler.h"

#include "MarbleDebug.h"
#include "KmlElementDictionary.h"
#include "GeoDataIconStyle.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataLink.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataSoundCue.h"
#include "GeoDataItemIcon.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( href )

GeoNode* KmlhrefTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_href)));

    GeoStackItem parentItem = parser.parentElement();

    QString content = parser.readElementText().trimmed();

    if ( parentItem.represents( kmlTag_Icon ) ) {
        // we need a more elaborate version of this part
        if ( parentItem.is<GeoDataIconStyle>() ) {
            parentItem.nodeAs<GeoDataIconStyle>()->setIconPath( content );
        } else if ( parentItem.is<GeoDataGroundOverlay>() ) {
            parentItem.nodeAs<GeoDataGroundOverlay>()->setIconFile( content );
        } else if ( parentItem.is<GeoDataPhotoOverlay>() ) {
            parentItem.nodeAs<GeoDataPhotoOverlay>()->setIconFile( content );
        } else if ( parentItem.is<GeoDataScreenOverlay>() ) {
            parentItem.nodeAs<GeoDataScreenOverlay>()->setIconFile( content );
        }
    } else if ( parentItem.represents( kmlTag_ItemIcon ) ) {
        parentItem.nodeAs<GeoDataItemIcon>()->setIconPath( content );
    } else if ( parentItem.is<GeoDataLink>() ) {
        parentItem.nodeAs<GeoDataLink>()->setHref( content );
    } else if ( parentItem.is<GeoDataSoundCue>() ) {
        parentItem.nodeAs<GeoDataSoundCue>()->setHref( content );
    }

    return nullptr;
}

}
}
