// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmlCameraTagWriter.h"

#include "GeoDataCamera.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlObjectTagWriter.h"

#include <QDateTime>

namespace Marble
{

static GeoTagWriterRegistrar s_writerCamera(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataCameraType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlCameraTagWriter());

bool KmlCameraTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto camera = static_cast<const GeoDataCamera *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Camera));
    KmlObjectTagWriter::writeIdentifiers(writer, camera);

    if (camera->timeStamp().when().isValid()) {
        writer.writeStartElement("gx:TimeStamp");
        writer.writeElement(QStringLiteral("when"), camera->timeStamp().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    if (camera->timeSpan().isValid()) {
        writer.writeStartElement("gx:TimeSpan");
        if (camera->timeSpan().begin().when().isValid())
            writer.writeElement(QStringLiteral("begin"), camera->timeSpan().begin().when().toString(Qt::ISODate));
        if (camera->timeSpan().end().when().isValid())
            writer.writeElement(QStringLiteral("end"), camera->timeSpan().end().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    if (camera->longitude() != 0.0) {
        writer.writeElement(QStringLiteral("longitude"), QString::number(camera->longitude(GeoDataCoordinates::Degree), 'f', 10));
    }
    if (camera->latitude() != 0.0) {
        writer.writeElement(QStringLiteral("latitude"), QString::number(camera->latitude(GeoDataCoordinates::Degree), 'f', 10));
    }
    writer.writeOptionalElement(QStringLiteral("altitude"), camera->altitude());
    writer.writeOptionalElement(QStringLiteral("roll"), camera->roll());
    writer.writeOptionalElement(QStringLiteral("tilt"), camera->tilt());
    writer.writeOptionalElement(QStringLiteral("heading"), camera->heading());
    KmlGroundOverlayWriter::writeAltitudeMode(writer, camera->altitudeMode());

    writer.writeEndElement();

    return true;
}

}
