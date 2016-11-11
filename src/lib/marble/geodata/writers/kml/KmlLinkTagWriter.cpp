//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Adrian Draghici <draghici.adrian.b@gmail.com>
//

#include "KmlLinkTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLink(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLinkType,
                                     kml::kmlTag_nameSpaceOgc22 ),
        new KmlLinkTagWriter );

bool KmlLinkTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataLink *link = static_cast<const GeoDataLink*>( node );

    writer.writeStartElement( kml::kmlTag_Link );
    KmlObjectTagWriter::writeIdentifiers( writer, link );

    writer.writeElement( kml::kmlTag_href, link->href() );

    QString const refreshMode = refreshModeToString( link->refreshMode() );
    writer.writeOptionalElement( kml::kmlTag_refreshMode, refreshMode, "onChange" );

    writer.writeElement( kml::kmlTag_refreshInterval, QString::number( link->refreshInterval() ) );

    QString const viewRefreshMode = viewRefreshModeToString( link->viewRefreshMode() );
    writer.writeOptionalElement( kml::kmlTag_viewRefreshMode, viewRefreshMode, "never" );

    writer.writeElement( kml::kmlTag_viewRefreshTime, QString::number( link->viewRefreshTime() ) );

    writer.writeElement( kml::kmlTag_viewBoundScale, QString::number( link->viewBoundScale() ) );

    writer.writeOptionalElement( kml::kmlTag_viewFormat, link->viewFormat());

    writer.writeOptionalElement( kml::kmlTag_httpQuery, link->httpQuery());

    writer.writeEndElement();

    return true;
}

QString KmlLinkTagWriter::refreshModeToString( GeoDataLink::RefreshMode refreshMode)
{
    switch (refreshMode)
    {
    case GeoDataLink::OnInterval:   return "onInterval";
    case GeoDataLink::OnExpire:     return "onExpire";
    default:                        return "onChange";
    }
}

QString KmlLinkTagWriter::viewRefreshModeToString( GeoDataLink::ViewRefreshMode viewRefreshMode)
{
    switch (viewRefreshMode)
    {
    case GeoDataLink::OnStop:       return "onStop";
    case GeoDataLink::OnRequest:    return "onRequest";
    case GeoDataLink::OnRegion:     return "onRegion";
    default:                        return "never";
    }
}

}
