//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
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

    writer.writeStartElement( kml::kmlTag_textColor );
    writer.writeCharacters( KmlColorStyleTagWriter::formatColor( balloonStyle->textColor() ) );
    writer.writeEndElement();

    writer.writeStartElement( kml::kmlTag_bgColor );
    writer.writeCharacters( KmlColorStyleTagWriter::formatColor( balloonStyle->backgroundColor() ) );
    writer.writeEndElement();

    QString textString;
    textString = balloonStyle->text();
    writer.writeStartElement( kml::kmlTag_text );
    writer.writeCharacters(textString);
    writer.writeEndElement();

    QString displayModeString;
    switch ( balloonStyle->displayMode() )
    {
        case GeoDataBalloonStyle::Hide:
        displayModeString = "hide";
        break;

        default:
        displayModeString = "default";
    }

    writer.writeStartElement( kml::kmlTag_displayMode );
    writer.writeCharacters( displayModeString );
    writer.writeEndElement();

    writer.writeEndElement();
    return true;
}

}
