// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#include "OsmTagWriter.h"

#include "GeoWriter.h"
#include "MarbleGlobal.h"
#include "OsmElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerOsm(GeoTagWriter::QualifiedName({}, QString::fromLatin1(osm::osmTag_version06)), new OsmTagWriter());

bool OsmTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    Q_UNUSED(node);

    writer.writeStartElement("osm");
    writer.writeAttribute("version", QString::fromUtf8(osm::osmTag_version06));
    writer.writeAttribute("generator", QLatin1StringView("Marble ") + QString::fromLatin1(MARBLE_VERSION_STRING));

    return true;
}

}
