// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlHeadTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneTypes.h"
#include "GeoSceneZoom.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerHead(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoSceneTypes::GeoSceneHeadType),
                                                                      QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                          new DgmlHeadTagWriter());

bool DgmlHeadTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto head = static_cast<const GeoSceneHead *>(node);
    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Head));
    writer.writeElement(QStringLiteral("name"), head->name());
    writer.writeElement(QStringLiteral("target"), head->target());
    writer.writeElement(QStringLiteral("theme"), head->theme());
    writer.writeElement(QStringLiteral("visible"), head->visible() ? QStringLiteral("true") : QStringLiteral("false"));
    writer.writeStartElement("description");
    writer.writeCDATA(head->description());
    writer.writeEndElement();

    const auto &icon = static_cast<const GeoSceneIcon &>(*head->icon());
    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Icon));
    writer.writeAttribute("pixmap", icon.pixmap());
    writer.writeEndElement();

    const auto &zoom = static_cast<const GeoSceneZoom &>(*head->zoom());
    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Zoom));
    writer.writeElement(QStringLiteral("discrete"), zoom.discrete() ? QStringLiteral("true") : QStringLiteral("false"));
    writer.writeTextElement("minimum", QString::number(zoom.minimum()));
    writer.writeTextElement("maximum", QString::number(zoom.maximum()));
    writer.writeEndElement();

    writer.writeEndElement();

    return true;
}

}
