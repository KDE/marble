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
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "MarbleDebug.h"
#include "GeoDataExtendedData.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataSchema.h"

#include "GeoDataTypes.h"

#include <QVector>

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataDocumentType,
                                                                            kml::kmlTag_nameSpaceOgc22),
                                               new KmlDocumentTagWriter() );

KmlDocumentTagWriter::KmlDocumentTagWriter() :
  KmlFeatureTagWriter( kml::kmlTag_Document )
{
  // nothing to do
}

bool KmlDocumentTagWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataDocument *document = static_cast<const GeoDataDocument*>(node);

    foreach( const GeoDataStyle::ConstPtr &style, document->styles() ) {
        writeElement( style.data(), writer );
    }
    foreach( const GeoDataStyleMap &map, document->styleMaps() ) {
        writeElement( &map, writer );
    }
    foreach( const GeoDataSchema &schema, document->schemas() ) {
        writeElement( &schema, writer );
    }

    QVector<GeoDataFeature*>::ConstIterator it =  document->constBegin();
    QVector<GeoDataFeature*>::ConstIterator const end = document->constEnd();

    for ( ; it != end; ++it ) {
        writeElement( *it, writer );
    }

    return true;
}

}
