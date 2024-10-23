// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlDocumentTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneLegend.h"
#include "GeoSceneMap.h"
#include "GeoSceneSettings.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument(GeoTagWriter::QualifiedName(QStringLiteral("GeoSceneDocument"), QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                              new DgmlDocumentTagWriter());

bool DgmlDocumentTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto document = static_cast<const GeoSceneDocument *>(node);

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Document));

    const GeoSceneHead *head = document->head();
    writeElement(head, writer);

    const GeoSceneMap *map = document->map();
    writeElement(map, writer);

    const GeoSceneSettings *settings = document->settings();
    writeElement(settings, writer);

    const GeoSceneLegend *legend = document->legend();
    writeElement(legend, writer);

    writer.writeEndDocument();
    return true;
}

}
