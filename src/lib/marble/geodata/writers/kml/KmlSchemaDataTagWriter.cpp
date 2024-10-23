// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "KmlSchemaDataTagWriter.h"

#include <QString>

#include "KmlElementDictionary.h"

#include "GeoWriter.h"

#include "GeoDataTypes.h"

#include "GeoDataSchemaData.h"
#include "GeoDataSimpleData.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSchemaData(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataSchemaDataType),
                                                                            QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                                new KmlSchemaDataTagWriter);

bool KmlSchemaDataTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto schemaData = static_cast<const GeoDataSchemaData *>(node);
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_SchemaData));
    QString schemaUrl = schemaData->schemaUrl();
    writer.writeAttribute("schemaUrl", schemaUrl);

    for (const GeoDataSimpleData &data : schemaData->simpleDataList()) {
        writeElement(&data, writer);
    }
    writer.writeEndElement();

    return true;
}

}
