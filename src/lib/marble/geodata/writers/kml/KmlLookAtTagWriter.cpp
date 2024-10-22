// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmlLookAtTagWriter.h"

#include "GeoDataLookAt.h"
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

static GeoTagWriterRegistrar s_writerLookAt(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataLookAtType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlLookAtTagWriter());

bool KmlLookAtTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto lookAt = static_cast<const GeoDataLookAt *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_LookAt));
    KmlObjectTagWriter::writeIdentifiers(writer, lookAt);

    if (lookAt->timeStamp().when().isValid()) {
        writer.writeStartElement("gx:TimeStamp");
        writer.writeElement(QStringLiteral("when"), lookAt->timeStamp().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    if (lookAt->timeSpan().isValid()) {
        writer.writeStartElement("gx:TimeSpan");
        if (lookAt->timeSpan().begin().when().isValid())
            writer.writeElement(QStringLiteral("begin"), lookAt->timeSpan().begin().when().toString(Qt::ISODate));
        if (lookAt->timeSpan().end().when().isValid())
            writer.writeElement(QStringLiteral("end"), lookAt->timeSpan().end().when().toString(Qt::ISODate));
        writer.writeEndElement();
    }

    writer.writeOptionalElement(QStringLiteral("longitude"), QString::number(lookAt->longitude(GeoDataCoordinates::Degree), 'f', 10));
    writer.writeOptionalElement(QStringLiteral("latitude"), QString::number(lookAt->latitude(GeoDataCoordinates::Degree), 'f', 10));
    writer.writeOptionalElement(QStringLiteral("altitude"), QString::number(lookAt->altitude(), 'f', 10));
    writer.writeOptionalElement(QStringLiteral("range"), QString::number(lookAt->range(), 'f', 10));
    KmlGroundOverlayWriter::writeAltitudeMode(writer, lookAt->altitudeMode());

    writer.writeEndElement();

    return true;
}

}
