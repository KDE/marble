//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyatf@gmail.com>
//

#include "KmlTimeSpanWriter.h"

#include "GeoDataTimeSpan.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataTimeSpanType,
				 kml::kmlTag_nameSpace22 ),
    new KmlTimeSpanWriter );

bool KmlTimeSpanWriter::write( const GeoNode *node,
			       GeoWriter& writer ) const
{
    const GeoDataTimeSpan *timespan =
	static_cast<const GeoDataTimeSpan*>( node );

    writer.writeStartElement( kml::kmlTag_TimeSpan );

    writer.writeTextElement( "begin",
			     timespan->begin().toString(Qt::ISODate) );
    writer.writeTextElement( "end",
			     timespan->end().toString(Qt::ISODate) );

    writer.writeEndElement();

    return true;
}

}

