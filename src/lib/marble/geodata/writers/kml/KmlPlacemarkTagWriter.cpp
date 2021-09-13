// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#include "KmlPlacemarkTagWriter.h"

#include "KmlElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTypes.h"
#include "GeoDataPolygon.h"
#include "GeoWriter.h"
#include "GeoDataStyle.h"

namespace Marble
{

//needs to handle a specific doctype. different versions different writer classes?
//don't use the tag dictionary for tag names, because with the writer we are using
// the object type strings instead
//FIXME: USE object strings provided by idis
static GeoTagWriterRegistrar s_writerPlacemark( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataPlacemarkType,
                                                kml::kmlTag_nameSpaceOgc22), new KmlPlacemarkTagWriter() );

bool KmlPlacemarkTagWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark*>(node);

    writer.writeOptionalElement( kml::kmlTag_styleUrl, placemark->styleUrl() );
    if ( placemark->styleUrl().isEmpty() && placemark->customStyle() ) {
        writeElement( placemark->customStyle().data(), writer );
    }

    if( placemark->geometry() ) {
        writeElement( placemark->geometry(), writer );
    }

    if( placemark->isBalloonVisible() ){
        QString string;
        string.setNum( 1 );
        writer.writeElement( kml::kmlTag_nameSpaceGx22, kml::kmlTag_balloonVisibility, string );
    }

    return true;
}

KmlPlacemarkTagWriter::KmlPlacemarkTagWriter() :
  KmlFeatureTagWriter( kml::kmlTag_Placemark )
{
  // nothing to do
}

}
