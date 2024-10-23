// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlStyleMapTagWriter.h"

#include "GeoDataStyleMap.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerStyleMap(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataStyleMapType),
                                                                          QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                              new KmlStyleMapTagWriter);

bool KmlStyleMapTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto map = static_cast<const GeoDataStyleMap *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_StyleMap));
    KmlObjectTagWriter::writeIdentifiers(writer, map);

    QMapIterator<QString, QString> iter(*map);
    while (iter.hasNext()) {
        iter.next();
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Pair));
        writer.writeElement(QString::fromLatin1(kml::kmlTag_key), iter.key());
        writer.writeElement(QString::fromLatin1(kml::kmlTag_styleUrl), iter.value());
        writer.writeEndElement();
    }

    writer.writeEndElement();

    return true;
}

}
