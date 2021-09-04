// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyatf@gmail.com>
//

#include "KmlTimeSpanWriter.h"

#include "GeoDataTimeSpan.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlTimeStampTagWriter.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataTimeSpanType,
				 kml::kmlTag_nameSpaceOgc22 ),
    new KmlTimeSpanWriter );

bool KmlTimeSpanWriter::write( const GeoNode *node,
			       GeoWriter& writer ) const
{
    Q_ASSERT(dynamic_cast<const GeoDataTimeSpan *>(node));
    const GeoDataTimeSpan *timespan = static_cast<const GeoDataTimeSpan *>(node);

    writer.writeStartElement( kml::kmlTag_TimeSpan );
    KmlObjectTagWriter::writeIdentifiers( writer, timespan );

    writer.writeTextElement( "begin", KmlTimeStampTagWriter::toString( timespan->begin() ) );
    writer.writeTextElement( "end", KmlTimeStampTagWriter::toString( timespan->end() ) );

    writer.writeEndElement();

    return true;
}

}

