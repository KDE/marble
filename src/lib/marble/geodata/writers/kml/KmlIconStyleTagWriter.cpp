//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlIconStyleTagWriter.h"

#include "GeoDataIconStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerIconStyle(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataIconStyleType,
                                 kml::kmlTag_nameSpace22 ),
        new KmlIconStyleTagWriter );

KmlIconStyleTagWriter::KmlIconStyleTagWriter() : KmlColorStyleTagWriter( kml::kmlTag_IconStyle )
{
    // nothing to do
}

bool KmlIconStyleTagWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataIconStyle *style = static_cast<const GeoDataIconStyle*>( node );

    writer.writeElement( kml::kmlTag_scale, QString::number( style->scale(), 'f' ) );

    writer.writeStartElement( kml::kmlTag_Icon );
    writer.writeStartElement( kml::kmlTag_href );
    writer.writeCharacters( style->iconPath() );
    writer.writeEndElement();
    writer.writeEndElement();

    writer.writeStartElement( kml::kmlTag_hotSpot );
    GeoDataHotSpot::Units xunits, yunits;
    QPointF const hotSpot = style->hotSpot( xunits, yunits );
    writer.writeAttribute( "x", QString::number( hotSpot.x(), 'f' ) );
    writer.writeAttribute( "y", QString::number( hotSpot.y(), 'f' ) );

    writer.writeAttribute( "xunits", unitString( xunits ) );
    writer.writeAttribute( "yunits", unitString( yunits ) );
    writer.writeEndElement();

    return true;
}

QString KmlIconStyleTagWriter::unitString(GeoDataHotSpot::Units unit) const
{
    switch (unit) {
    case GeoDataHotSpot::Pixels: return "pixels";
    case GeoDataHotSpot::InsetPixels: return "insetPixels";
    case GeoDataHotSpot::Fraction: return "fraction";
    }

    return "fraction";
}

}
