//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "KmlPlacemarkTagWriter.h"
#include "KmlElementDictionary.h"
//FIXME:should the GeoDataTypes enum be in the GeoDocument?
#include "GeoDocument.h"
#include "GeoWriter.h"

namespace Marble{

//needs to handle a specific doctype. different versions different writer classes?
//don't use the tag dictionary for tag names, because with the writer we are using
// the object type strings instead
//FIXME: USE object strings provided by idis
static GeoTagWriterRegistrar s_writerPlacemark( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataPlacemarkType,
                                                                            kml::kmlTag_nameSpace22),
                                               new KmlPlacemarkTagWriter() );


bool KmlPlacemarkTagWriter::write( GeoNode &node, GeoWriter& writer ) const
{
    writer.writeStartElement( kml::kmlTag_Placemark );
    //Write the actual important stuff!
    writer.writeEndElement();
    return true;
}

}
