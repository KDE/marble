// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlSectionTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneIcon.h"
#include "GeoSceneItem.h"
#include "GeoSceneSection.h"
#include "GeoSceneTypes.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSection(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoSceneTypes::GeoSceneSectionType),
                                                                         QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                             new DgmlSectionTagWriter());

bool DgmlSectionTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto section = static_cast<const GeoSceneSection *>(node);

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Section));
    writer.writeAttribute(QStringLiteral("name"), section->name());
    writer.writeAttribute("checkable", section->checkable() ? QStringLiteral("true") : QStringLiteral("false"));
    writer.writeAttribute("connect", section->connectTo());
    writer.writeAttribute("spacing", QString::number(section->spacing()));
    writer.writeElement(QString::fromLatin1(dgml::dgmlTag_Heading), section->heading());

    for (int i = 0; i < section->items().count(); ++i) {
        GeoSceneItem *item = section->items().at(i);
        writeElement(item, writer);
    }

    writer.writeEndElement();
    return true;
}

}
