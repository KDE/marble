// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Javier Becerra <javier@auva.es>
//

#include "KmlTimeStampTagWriter.h"

#include "GeoDataTimeStamp.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

#include <QDateTime>

namespace Marble
{

static GeoTagWriterRegistrar s_writerTimeStamp(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataTimeStampType),
                                                                           QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                               new KmlTimeStampTagWriter());

bool KmlTimeStampTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto timestamp = static_cast<const GeoDataTimeStamp *>(node);

    if (timestamp->when().isValid()) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_TimeStamp));
        KmlObjectTagWriter::writeIdentifiers(writer, timestamp);

        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_when));
        writer.writeCharacters(toString(*timestamp));
        writer.writeEndElement();

        writer.writeEndElement();
    }
    return true;
}

QString KmlTimeStampTagWriter::toString(const GeoDataTimeStamp &timestamp)
{
    switch (timestamp.resolution()) {
    case GeoDataTimeStamp::SecondResolution:
        return timestamp.when().toString(Qt::ISODate);
    case GeoDataTimeStamp::DayResolution:
        return timestamp.when().toString(QStringLiteral("yyyy-MM-dd"));
    case GeoDataTimeStamp::MonthResolution:
        return timestamp.when().toString(QStringLiteral("yyyy-MM"));
    case GeoDataTimeStamp::YearResolution:
        return timestamp.when().toString(QStringLiteral("yyyy"));
    }

    Q_ASSERT(false && "not reachable");
    return {};
}

}
