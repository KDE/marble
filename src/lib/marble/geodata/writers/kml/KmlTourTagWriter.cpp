// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#include "KmlTourTagWriter.h"

#include "GeoDataTour.h"
#include "GeoDataTypes.h"
#include "GeoDataPlaylist.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerTour(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataTourType,
                                     kml::kmlTag_nameSpaceOgc22 ),
        new KmlTourTagWriter );

bool KmlTourTagWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const  GeoDataTour* tour = static_cast<const GeoDataTour*>( node );
    if ( tour->playlist() ) {
        writeElement( tour->playlist(), writer );
    }
    return true;
}

KmlTourTagWriter::KmlTourTagWriter() :
    KmlFeatureTagWriter( "gx:Tour" )
{
    // nothing to do
}

}
