// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlSettingsTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneProperty.h"
#include "GeoSceneSettings.h"
#include "GeoSceneTypes.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSettings(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoSceneTypes::GeoSceneSettingsType),
                                                                          QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                              new DgmlSettingsTagWriter());

bool DgmlSettingsTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto settings = static_cast<const GeoSceneSettings *>(node);
    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Settings));

    for (int i = 0; i < settings->allProperties().count(); ++i) {
        const GeoSceneProperty *property = settings->allProperties().at(i);
        writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Property));
        writer.writeAttribute(QStringLiteral("name"), property->name());
        writer.writeElement(QString::fromLatin1(dgml::dgmlTag_Value), property->defaultValue() ? QStringLiteral("true") : QStringLiteral("false"));
        writer.writeElement(QString::fromLatin1(dgml::dgmlTag_Available), property->available() ? QStringLiteral("true") : QStringLiteral("false"));
        writer.writeEndElement();
    }

    writer.writeEndElement();
    return true;
}

}
