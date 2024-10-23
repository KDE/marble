// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2014 Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlPointTagWriter.h"

#include "GeoDataPoint.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerPoint(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataPointType),
                                                                       QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                           new KmlPointTagWriter());

bool KmlPointTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto point = static_cast<const GeoDataPoint *>(node);

    if (!point->coordinates().isValid()) {
        return true;
    }

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Point));
    KmlObjectTagWriter::writeIdentifiers(writer, point);
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_extrude), QString::number(point->extrude()), QStringLiteral("0"));
    writer.writeStartElement("coordinates");

    // FIXME: this should be using the GeoDataCoordinates::toString but currently
    //  it is not including the altitude and is adding an extra space after commas

    QString coordinateString = QString::number(point->coordinates().longitude(GeoDataCoordinates::Degree), 'f', 10) + QLatin1Char(',')
        + QString::number(point->coordinates().latitude(GeoDataCoordinates::Degree), 'f', 10);

    if (point->coordinates().altitude()) {
        coordinateString += QLatin1Char(',') + QString::number(point->coordinates().altitude(), 'f', 10);
    }

    writer.writeCharacters(coordinateString);
    writer.writeEndElement();

    KmlGroundOverlayWriter::writeAltitudeMode(writer, point->altitudeMode());

    writer.writeEndElement();

    return true;
}

}
