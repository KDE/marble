// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlItemTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneIcon.h"
#include "GeoSceneItem.h"
#include "GeoSceneSection.h"
#include "GeoSceneTypes.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerItem(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoSceneTypes::GeoSceneItemType),
                                                                      QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                          new DgmlItemTagWriter());

bool DgmlItemTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto item = static_cast<const GeoSceneItem *>(node);

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Item));
    writer.writeAttribute(QStringLiteral("name"), item->name());

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Icon));
    if (!item->icon()->pixmap().isEmpty()) {
        writer.writeAttribute("pixmap", item->icon()->pixmap());
    } else if (item->icon()->color().isValid()) {
        writer.writeAttribute(QStringLiteral("color"), item->icon()->color().name());
    }
    writer.writeEndElement();
    writer.writeElement(QStringLiteral("text"), item->text());

    writer.writeEndElement();
    return true;
}

}
