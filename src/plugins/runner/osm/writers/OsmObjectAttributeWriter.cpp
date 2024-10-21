// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#include "OsmObjectAttributeWriter.h"
#include "GeoWriter.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

void OsmObjectAttributeWriter::writeAttributes(const OsmPlacemarkData &osmData, GeoWriter &writer)
{
    writer.writeAttribute("id", QString::number(osmData.id()));
    writer.writeOptionalAttribute(QStringLiteral("action"), osmData.action());
    writer.writeOptionalAttribute(QStringLiteral("changeset"), osmData.changeset());
    writer.writeOptionalAttribute(QStringLiteral("timestamp"), osmData.timestamp());
    writer.writeOptionalAttribute(QStringLiteral("uid"), osmData.uid());
    writer.writeOptionalAttribute(QStringLiteral("user"), osmData.user());
    writer.writeOptionalAttribute(QStringLiteral("version"), osmData.version());
    writer.writeOptionalAttribute(QStringLiteral("visible"), osmData.isVisible());
}

}
