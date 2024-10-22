// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlLegendTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSection.h"
#include "GeoSceneTypes.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLegend(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoSceneTypes::GeoSceneLegendType),
                                                                        QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                            new DgmlLegendTagWriter());

bool DgmlLegendTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto legend = static_cast<const GeoSceneLegend *>(node);

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Legend));
    for (int i = 0; i < legend->sections().count(); ++i) {
        const GeoSceneSection *section = legend->sections().at(i);
        writeElement(section, writer);
    }
    writer.writeEndElement();
    return true;
}

}
