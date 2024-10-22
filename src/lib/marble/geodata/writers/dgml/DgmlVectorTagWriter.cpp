// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlVectorTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneTypes.h"
#include "GeoSceneVector.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerVector(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoSceneTypes::GeoSceneVectorType),
                                                                        QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                            new DgmlVectorTagWriter());

bool DgmlVectorTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto vector = static_cast<const GeoSceneVector *>(node);
    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Vector));
    writer.writeAttribute(QStringLiteral("name"), vector->name());
    writer.writeAttribute("feature", vector->feature());

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_SourceFile));
    writer.writeAttribute(QStringLiteral("format"), vector->fileFormat());
    writer.writeCharacters(vector->sourceFile());
    writer.writeEndElement();

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Pen));
    writer.writeAttribute(QStringLiteral("color"), vector->pen().color().name());
    writer.writeEndElement();

    writer.writeEndElement();
    return true;
}

}
