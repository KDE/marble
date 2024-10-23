// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "KmlTrackWriter.h"

#include "GeoDataCoordinates.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

#include <QDateTime>

using namespace Marble;

static GeoTagWriterRegistrar s_writerPoint(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataTrackType),
                                                                       QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                           new KmlTrackWriter());

bool KmlTrackWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto track = static_cast<const GeoDataTrack *>(node);

    writer.writeStartElement("gx:Track");
    KmlObjectTagWriter::writeIdentifiers(writer, track);

    int points = track->size();
    for (int i = 0; i < points; i++) {
        writer.writeElement(QStringLiteral("when"), track->whenList().at(i).toString(Qt::ISODate));

        qreal lon, lat, alt;
        track->coordinatesList().at(i).geoCoordinates(lon, lat, alt, GeoDataCoordinates::Degree);
        const QString coord =
            QString::number(lon, 'f', 10) + QLatin1Char(' ') + QString::number(lat, 'f', 10) + QLatin1Char(' ') + QString::number(alt, 'f', 10);

        writer.writeElement(QStringLiteral("gx:coord"), coord);
    }
    writer.writeEndElement();

    return true;
}
