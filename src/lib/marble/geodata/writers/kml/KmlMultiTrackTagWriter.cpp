// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>
//

#include "KmlMultiTrackTagWriter.h"

#include "GeoDataMultiTrack.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerMultiTrack(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataMultiTrackType),
                                                                            QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                                new KmlMultiTrackTagWriter);

bool KmlMultiTrackTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto geometry = static_cast<const GeoDataMultiTrack *>(node);

    writer.writeStartElement("gx:MultiTrack");
    KmlObjectTagWriter::writeIdentifiers(writer, geometry);

    for (int i = 0; i < geometry->size(); ++i) {
        writeElement(&geometry->at(i), writer);
    }

    writer.writeEndElement();

    return true;
}

}
