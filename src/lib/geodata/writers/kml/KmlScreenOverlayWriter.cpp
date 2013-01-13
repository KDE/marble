//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlScreenOverlayWriter.h"

#include "GeoDataScreenOverlay.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "GeoTagWriter.h"
#include "KmlOverlayTagWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{
static GeoTagWriterRegistrar s_writerLookAt(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataScreenOverlayType,
                                     kml::kmlTag_nameSpace22 ),
        new KmlScreenOverlayWriter );

KmlScreenOverlayWriter::KmlScreenOverlayWriter() : KmlOverlayTagWriter( kml::kmlTag_ScreenOverlay )
{
    // nothing to do
}


bool KmlScreenOverlayWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataScreenOverlay *screenOverlay = static_cast<const GeoDataScreenOverlay*>( node );

    QString overlayXString;
    QString overlayXunitString;
    QString overlayYString;
    QString overlayYunitString;

    overlayXString = QString::number(screenOverlay->overlayXY().x());
    overlayXunitString = QString::number(screenOverlay->overlayXY().xunit());
    overlayYString = QString::number(screenOverlay->overlayXY().y());
    overlayYunitString = QString::number(screenOverlay->overlayXY().yunit());

    writer.writeStartElement(kml::kmlTag_overlayXY);
    writer.writeAttribute("x", overlayXString);
    writer.writeAttribute("xunit", overlayXunitString);
    writer.writeAttribute("y", overlayYString);
    writer.writeAttribute("yunit", overlayYunitString);
    writer.writeEndElement();

    QString screenXString;
    QString screenXunitString;
    QString screenYString;
    QString screenYunitString;

    screenXString = QString::number(screenOverlay->screenXY().x());
    screenXunitString = QString::number(screenOverlay->screenXY().xunit());
    screenYString = QString::number(screenOverlay->screenXY().y());
    screenYunitString = QString::number(screenOverlay->screenXY().yunit());

    writer.writeStartElement(kml::kmlTag_screenXY);
    writer.writeAttribute("x", screenXString);
    writer.writeAttribute("xunit", screenXunitString);
    writer.writeAttribute("y", screenYString);
    writer.writeAttribute("yunit", screenYunitString);
    writer.writeEndElement();

    QString sizeXString;
    QString sizeXunitString;
    QString sizeYString;
    QString sizeYunitString;

    sizeXString = QString::number(screenOverlay->size().x());
    sizeXunitString = QString::number(screenOverlay->size().xunit());
    sizeYString = QString::number(screenOverlay->size().y());
    sizeYunitString = QString::number(screenOverlay->size().yunit());

    writer.writeStartElement(kml::kmlTag_size);
    writer.writeAttribute("x", sizeXString);
    writer.writeAttribute("xunit", sizeXunitString);
    writer.writeAttribute("y", sizeYString);
    writer.writeAttribute("yunit", sizeYunitString);
    writer.writeEndElement();

    QString rotationXString;
    QString rotationXunitString;
    QString rotationYString;
    QString rotationYunitString;

    rotationXString = QString::number(screenOverlay->rotationXY().x());
    rotationXunitString = QString::number(screenOverlay->rotationXY().xunit());
    rotationYString = QString::number(screenOverlay->rotationXY().y());
    rotationYunitString = QString::number(screenOverlay->rotationXY().yunit());

    writer.writeStartElement(kml::kmlTag_size);
    writer.writeAttribute("x", rotationXString);
    writer.writeAttribute("xunit", rotationXunitString);
    writer.writeAttribute("y", rotationYString);
    writer.writeAttribute("yunit", rotationYunitString);
    writer.writeEndElement();

    QString rotationString;
    rotationString = QString::number(screenOverlay->rotation());
    writer.writeStartElement(kml::kmlTag_rotation);
    writer.writeCharacters(rotationString);
    writer.writeEndElement();

    return true;

}

}

