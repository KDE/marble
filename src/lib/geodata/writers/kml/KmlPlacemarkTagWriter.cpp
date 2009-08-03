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
//don't use the tag dictionary for tag names, because with the writer we are using
// the object type strings instead
//FIXME: USE object strings provided by idis
static GeoTagWriterRegistrar s_writerPlacemark( GeoTagWriter::QualifiedName("ObjectType",
                                                                            kml::kmlTag_nameSpace20),
                                               new KmlPlacemarkTagWriter() );


bool KmlPlacemarkTagWriter::write( GeoNode &node, GeoWriter& writer )
{
    //write some stuff to the GeoWriter... which is just an XMLStream writer!
    return false;
}

}
