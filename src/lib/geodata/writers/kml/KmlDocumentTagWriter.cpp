//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "KmlDocumentTagWriter.h"

#include "GeoDocument.h"
#include "GeoDataDocument.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "GeoDataObject.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "MarbleDebug.h"

#include "GeoDataTypes.h"

#include <QtCore/QVector>

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataDocumentType,
                                                                            kml::kmlTag_nameSpace22),
                                               new KmlDocumentTagWriter() );

bool KmlDocumentTagWriter::write( const GeoDataObject &node, GeoWriter& writer ) const
{
    const GeoDataDocument &document = static_cast<const GeoDataDocument&>(node);

    // when a document has only one feature and no styling
    // the document tag is excused
    if( (document.styles().count() == 0)
        && (document.styleMaps().count() == 0)
        && (document.featureList().count() == 1) ) {
        writeElement( *document.featureList()[0], writer );
        return true;
    }

    writer.writeStartElement( kml::kmlTag_Document );

    foreach( const GeoDataStyle &style, document.styles() ) {
        writeElement( style, writer );
    }
    foreach( const GeoDataStyleMap &map, document.styleMaps() ) {
        writeElement( map, writer );
    }

    writer.writeOptionalElement( "name", document.name() );
    writer.writeOptionalElement( "address", document.address() );

    QVector<GeoDataFeature*>::ConstIterator it =  document.constBegin();
    QVector<GeoDataFeature*>::ConstIterator const end = document.constEnd();

    for ( ; it != end; ++it ) {
        writeElement( (**it), writer );
    }

    //Write the actual important stuff!
    writer.writeEndElement();
    return true;
}

}
