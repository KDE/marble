//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlBalloonStyleTagWriter.h"

#include "GeoDataBalloonStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlColorStyleTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerBallonStyle( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataBalloonStyleType,
                                                                              kml::kmlTag_nameSpace22),
                                                  new KmlBalloonStyleTagWriter() );

bool KmlBalloonStyleTagWriter::write( const GeoNode *node,
                               GeoWriter& writer ) const
{
    const GeoDataBalloonStyle *balloonStyle = static_cast<const GeoDataBalloonStyle*>( node );
    writer.writeStartElement( kml::kmlTag_BalloonStyle );

    QString const textColor = KmlColorStyleTagWriter::formatColor( balloonStyle->textColor() );
    writer.writeOptionalElement( kml::kmlTag_textColor, textColor, "ff000000" );
    QString const backgroundColor = KmlColorStyleTagWriter::formatColor( balloonStyle->backgroundColor() );
    writer.writeOptionalElement( kml::kmlTag_bgColor, backgroundColor, "ffffffff" );

    QString const textString = balloonStyle->text();
    if ( textString.contains( QRegExp( "[<>&]" ) ) ) {
        writer.writeStartElement( kml::kmlTag_text );
        writer.writeCDATA( textString );
        writer.writeEndElement();
    } else {
        writer.writeOptionalElement( kml::kmlTag_text, textString );
    }

    QString const displayMode = balloonStyle->displayMode() == GeoDataBalloonStyle::Hide ? "hide" : "default";
    writer.writeOptionalElement( kml::kmlTag_displayMode, displayMode );

    writer.writeEndElement();
    return true;
}

}
