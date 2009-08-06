//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoWriter.h"

#include "GeoTagWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

GeoWriter::GeoWriter()
{
    //FIXME: work out a standard way to do this.
    m_documentType = kml::kmlTag_nameSpace22;
}

bool GeoWriter::write(QIODevice* device, const QList<GeoDataFeature> &features)
{
    setDevice( device );
    //FIXME: write the starting tags. Possibly register a tag handler to do this
    // with a null string as the object name?

    QListIterator<GeoDataFeature> it(features);

    while ( it.hasNext() ) {
        GeoDataFeature f = it.next();
        GeoTagWriter::QualifiedName name( f.nodeType(), m_documentType );
        const GeoTagWriter* writer = GeoTagWriter::recognizes( name );

        if( writer ) {
            if(!writer->write( f, (*this) ) ) {
                //something went wrong while writing
                return false;
            }
        } else {
            //do not have a handler for this element
            return false;
        }
    }
    return true;
}

bool GeoWriter::write( QIODevice *device, const GeoDataFeature &feature)
{
    QList<GeoDataFeature> list;
    list.append(feature);
    return write(device, list);
}

void GeoWriter::setDocumentType( const QString &documentType )
{
    m_documentType = documentType;
}

}
