// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlGeodataTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneTypes.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerGeodata(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoSceneTypes::GeoSceneGeodataType),
                                                                         QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                             new DgmlGeodataTagWriter());

bool DgmlGeodataTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto geodata = static_cast<const GeoSceneGeodata *>(node);
    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Geodata));
    writer.writeAttribute(QStringLiteral("name"), geodata->name());

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_SourceFile));
    writer.writeCharacters(geodata->sourceFile());
    writer.writeEndElement();

    writer.writeEndElement();
    return true;
}

}
