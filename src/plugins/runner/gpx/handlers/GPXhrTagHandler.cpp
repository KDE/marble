//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GPXhrTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataTrack.h"
#include "GeoDataSimpleArrayData.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER_GARMIN_TRACKPOINTEXT1(hr)

GeoNode* GPXhrTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( gpxTag_hr ) );
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataTrack>() )
    {
        GeoDataSimpleArrayData* arrayData = parentItem.nodeAs<GeoDataTrack>()
                        ->extendedData().simpleArrayData( "heartrate" );
        if (!arrayData) {
            arrayData = new GeoDataSimpleArrayData();
            QString name = parser.attribute( "name" ).trimmed();
            parentItem.nodeAs<GeoDataTrack>()->extendedData().setSimpleArrayData( "heartrate", arrayData );
        }
        QVariant value( parser.readElementText().toInt() );
        arrayData->append( value );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_hr << "> value" << value;
#endif
        return 0;
    }

    return 0;
}

} // namespace gpx

} // namespace Marble
