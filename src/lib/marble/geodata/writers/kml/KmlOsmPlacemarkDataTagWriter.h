//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_KMLOSMPLACEMARKDATATAGWRITER_H
#define MARBLE_KMLOSMPLACEMARKDATATAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class GeoDataFeature;
class OsmPlacemarkData;
class GeoDataGeometry;

/**
 * @brief The KmlOsmPlacemarkDataTagWriter class is not a standard GeoTagWriter extension
 * because the osm data fields have to be written in relation to the placemark's geometry
 */
class KmlOsmPlacemarkDataTagWriter
{
public:
    /**
     * @brief write function writes a custom XML schema made to store
     * OsmPlacemarkData in a valid KML context
     * @see https://developers.google.com/kml/documentation/extendeddata#opaquedata
     *
     * The custom schema has the following structure ( most complex one, for polygons ):
     *<Placemark>
     *    ...
     *    <ExtendedData xmlns:mx="http://marble.kde.org">
     *        <mx:OsmPlacemarkData id=...>
     *             <mx:tag k="key" v="value"/>
     *             .. more tags
     *
     *             <mx:member index="-1">     //outer boundary
     *                 <mx:OsmPlacemarkData id=...>
     *                 <mx:nd index="0">
     *                     <mx:OsmPlacemarkData id= .../> //nd #0
     *                 </nd>
     *                 <mx:nd index="1">
     *                     <mx:OsmPlacemarkData id= .../> //nd #1
     *                 </nd>
     *                 ... more nds
     *             </member>
     *
     *             <mx:member index="0">     //first inner boundary
     *             ...
     *             </member>
     *             ... more inner boundaries
     *
     *        </mx:OsmPlacemarkData>
     *    </ExtendedData>
     * </Placemark>
     */
     static bool write( const GeoDataFeature *feature, GeoWriter& writer );
     static bool writeOsmData( const GeoDataGeometry *geometry, const OsmPlacemarkData &osmData, GeoWriter& writer );
};

}

#endif

