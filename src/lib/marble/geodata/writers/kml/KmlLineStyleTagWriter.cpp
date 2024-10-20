// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlLineStyleTagWriter.h"

#include "GeoDataLineStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLineStyle(GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataLineStyleType, kml::kmlTag_nameSpaceOgc22),
                                               new KmlLineStyleTagWriter);

KmlLineStyleTagWriter::KmlLineStyleTagWriter()
    : KmlColorStyleTagWriter(kml::kmlTag_LineStyle)
{
    // nothing to do
}

bool KmlLineStyleTagWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    const auto style = static_cast<const GeoDataLineStyle *>(node);
    writer.writeOptionalElement("width", style->width(), 1.0f);
    return true;
}

bool KmlLineStyleTagWriter::isEmpty(const GeoNode *node) const
{
    const auto style = static_cast<const GeoDataLineStyle *>(node);
    return style->width() == 1.0;
}

}
