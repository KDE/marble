//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
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
                                 kml::kmlTag_nameSpaceOgc22 ),
        new KmlIconStyleTagWriter );

KmlIconStyleTagWriter::KmlIconStyleTagWriter() : KmlColorStyleTagWriter( kml::kmlTag_IconStyle )
{
    // nothing to do
}

bool KmlIconStyleTagWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataIconStyle *style = static_cast<const GeoDataIconStyle*>( node );

    if ( style->scale() != 1.0 ) {
        writer.writeElement( kml::kmlTag_scale, QString::number( style->scale(), 'f' ) );
    }

    if ( !style->iconPath().isEmpty() ) {
        writer.writeStartElement( kml::kmlTag_Icon );
        writer.writeStartElement( kml::kmlTag_href );
        writer.writeCharacters( style->iconPath() );
        writer.writeEndElement();
        writer.writeEndElement();
    }

    GeoDataHotSpot::Units xunits, yunits;
    QPointF const hotSpot = style->hotSpot( xunits, yunits );
    bool const emptyHotSpot = hotSpot.x() == 0.5 && hotSpot.y() == 0.5 &&
        xunits == GeoDataHotSpot::Fraction && yunits == GeoDataHotSpot::Fraction;
    if ( !emptyHotSpot ) {
        writer.writeStartElement( kml::kmlTag_hotSpot );
        if ( hotSpot.x() != 0.5 || xunits != GeoDataHotSpot::Fraction ) {
            writer.writeAttribute( "x", QString::number( hotSpot.x(), 'f' ) );
        }
        if ( hotSpot.y() != 0.5 || yunits != GeoDataHotSpot::Fraction ) {
            writer.writeAttribute( "y", QString::number( hotSpot.y(), 'f' ) );
        }

        if ( xunits != GeoDataHotSpot::Fraction ) {
            writer.writeAttribute( "xunits", unitString( xunits ) );
        }
        if ( yunits != GeoDataHotSpot::Fraction ) {
            writer.writeAttribute( "yunits", unitString( yunits ) );
        }
        writer.writeEndElement();
    }

    return true;
}

bool KmlIconStyleTagWriter::isEmpty( const GeoNode *node ) const
{
    const GeoDataIconStyle *style = static_cast<const GeoDataIconStyle*>( node );
    GeoDataHotSpot::Units xunits, yunits;
    QPointF const hotSpot = style->hotSpot( xunits, yunits );
    return style->iconPath().isEmpty() &&
            hotSpot.x() == 0.5 &&
            hotSpot.y() == 0.5 &&
            xunits == GeoDataHotSpot::Fraction &&
            yunits == GeoDataHotSpot::Fraction;
}

QString KmlIconStyleTagWriter::unitString(GeoDataHotSpot::Units unit)
{
    switch (unit) {
    case GeoDataHotSpot::Pixels: return "pixels";
    case GeoDataHotSpot::InsetPixels: return "insetPixels";
    case GeoDataHotSpot::Fraction: return "fraction";
    }

    return "fraction";
}

}
