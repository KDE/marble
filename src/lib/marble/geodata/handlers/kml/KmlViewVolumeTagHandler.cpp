// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlViewVolumeTagHandler.h"

#include "GeoDataPhotoOverlay.h"
#include "GeoDataViewVolume.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( ViewVolume )

GeoNode* KmlViewVolumeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_ViewVolume)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_PhotoOverlay ) ) {
        return &parentItem.nodeAs<GeoDataPhotoOverlay>()->viewVolume();
    }
    return nullptr;
}

}
}
