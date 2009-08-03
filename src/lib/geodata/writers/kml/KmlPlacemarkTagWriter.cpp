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

namespace Marble{

//needs to handle a specific doctype. different versions different writer classes?
static GeoTagWriterRegistrar s_writerPlacemark( GeoTagWriter::QualifiedName(kml::kmlTag_Placemark,
                                                                            kml::kmlTag_nameSpace20),
                                               new KmlPlacemarkTagWriter() );


bool KmlPlacemarkTagWriter::write( GeoNode &node, GeoWriter& writer )
{
    //write some stuff to the GeoWriter... which is just an XMLStream writer!
    return false;
}

}
