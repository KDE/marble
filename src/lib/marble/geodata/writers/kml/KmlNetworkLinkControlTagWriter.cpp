//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlNetworkLinkControlTagWriter.h"

#include "GeoDataNetworkLinkControl.h"
#include "GeoDataTypes.h"
#include "GeoDataUpdate.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

#include <QDateTime>

namespace Marble
{

static GeoTagWriterRegistrar s_writerNetworkLinkControl(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataNetworkLinkControlType,
                                     kml::kmlTag_nameSpaceOgc22 ),
        new KmlNetworkLinkControlTagWriter );

bool KmlNetworkLinkControlTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataNetworkLinkControl *networkLinkControl = static_cast<const GeoDataNetworkLinkControl*>( node );

    writer.writeStartElement( kml::kmlTag_NetworkLinkControl );
    writer.writeOptionalElement( kml::kmlTag_minRefreshPeriod, QString::number( networkLinkControl->minRefreshPeriod() ), "1" );
    writer.writeOptionalElement( kml::kmlTag_maxSessionLength, QString::number( networkLinkControl->maxSessionLength() ), "2" );
    writer.writeOptionalElement( kml::kmlTag_cookie, networkLinkControl->cookie() );
    writer.writeOptionalElement( kml::kmlTag_message, networkLinkControl->message() );
    writer.writeOptionalElement( kml::kmlTag_linkName, networkLinkControl->linkName() );
    writer.writeOptionalElement( kml::kmlTag_linkDescription, networkLinkControl->linkDescription() );

    writer.writeStartElement( kml::kmlTag_linkSnippet );

    if( networkLinkControl->maxLines() > 0 ) {
        writer.writeAttribute( "maxLines", QString::number( networkLinkControl->maxLines() ) );
    }

    writer.writeCharacters( networkLinkControl->linkSnippet() );
    writer.writeEndElement();

    writer.writeOptionalElement( kml::kmlTag_expires, networkLinkControl->expires().toString( Qt::ISODate ) );
    writeElement( &networkLinkControl->update(), writer );
    writer.writeEndElement();

    return true;
}

}
