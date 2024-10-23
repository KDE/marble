// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "KmlSchemaTagWriter.h"

#include "GeoDataSchema.h"
#include "GeoDataSimpleField.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSchema(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataSchemaType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlSchemaTagWriter);

bool KmlSchemaTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto schema = static_cast<const GeoDataSchema *>(node);
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Schema));
    KmlObjectTagWriter::writeIdentifiers(writer, schema);
    QString name = schema->schemaName();
    writer.writeAttribute(QStringLiteral("name"), name);

    for (const GeoDataSimpleField &simpleField : schema->simpleFields()) {
        writeElement(&simpleField, writer);
    }

    writer.writeEndElement();

    return true;
}

}
