/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

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

    return 0;
}

}
}
