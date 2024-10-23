// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlUpdateTagWriter.h"
#include "GeoDataAbstractView.h"
#include "GeoDataChange.h"
#include "GeoDataCreate.h"
#include "GeoDataDelete.h"
#include "GeoDataTypes.h"
#include "GeoDataUpdate.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerUpdate(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataUpdateType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlUpdateTagWriter);

bool KmlUpdateTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto update = static_cast<const GeoDataUpdate *>(node);
    KmlObjectTagWriter::writeIdentifiers(writer, update);
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Update));
    writer.writeElement(QString::fromLatin1(kml::kmlTag_targetHref), update->targetHref());

    if (update->change() && update->change()->size() > 0) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Change));
        QList<GeoDataFeature *>::ConstIterator it = update->change()->constBegin();
        QList<GeoDataFeature *>::ConstIterator const end = update->change()->constEnd();

        for (; it != end; ++it) {
            writeElement(*it, writer);
        }
        writer.writeEndElement();
    } else if (update->create() && update->create()->size() > 0) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Create));
        QList<GeoDataFeature *>::ConstIterator it = update->create()->constBegin();
        QList<GeoDataFeature *>::ConstIterator const end = update->create()->constEnd();

        for (; it != end; ++it) {
            writeElement(*it, writer);
        }
        writer.writeEndElement();
    } else if (update->getDelete() && update->getDelete()->size() > 0) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Delete));
        QList<GeoDataFeature *>::ConstIterator it = update->getDelete()->constBegin();
        QList<GeoDataFeature *>::ConstIterator const end = update->getDelete()->constEnd();

        for (; it != end; ++it) {
            writeElement(*it, writer);
        }
        writer.writeEndElement();
    }

    writer.writeEndElement();
    return true;
}

}
