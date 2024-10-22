// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#include "KmlExtendedDataTagWriter.h"

#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataSchemaData.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

#include <QHash>

namespace Marble
{

static GeoTagWriterRegistrar s_writerExtendedData(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataExtendedDataType),
                                                                              QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                                  new KmlExtendedDataTagWriter());

bool KmlExtendedDataTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto extended = static_cast<const GeoDataExtendedData *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_ExtendedData));

    QHash<QString, GeoDataData>::const_iterator begin = extended->constBegin();
    QHash<QString, GeoDataData>::const_iterator end = extended->constEnd();

    for (QHash<QString, GeoDataData>::const_iterator i = begin; i != end; ++i) {
        writeElement(&i.value(), writer);
    }

    for (const GeoDataSchemaData &schemaData : extended->schemaDataList()) {
        writeElement(&schemaData, writer);
    }

    writer.writeEndElement();

    return true;
}

}
