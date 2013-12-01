//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "KmlPlacemarkTagWriter.h"

#include "KmlElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"

namespace Marble
{

//needs to handle a specific doctype. different versions different writer classes?
//don't use the tag dictionary for tag names, because with the writer we are using
// the object type strings instead
//FIXME: USE object strings provided by idis
static GeoTagWriterRegistrar s_writerPlacemark( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataPlacemarkType,
                                                                            kml::kmlTag_nameSpace22),
                                               new KmlPlacemarkTagWriter() );


bool KmlPlacemarkTagWriter::writeMid( const GeoNode *node,
                                   GeoWriter& writer ) const
{
    const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark*>(node);

    writer.writeOptionalElement( kml::kmlTag_styleUrl, placemark->styleUrl() );

    if( placemark->geometry() ) {
        writeElement( placemark->geometry(), writer );
    }

    return true;
}

KmlPlacemarkTagWriter::KmlPlacemarkTagWriter() :
  KmlFeatureTagWriter( kml::kmlTag_Placemark )
{
  // nothing to do
}

}
