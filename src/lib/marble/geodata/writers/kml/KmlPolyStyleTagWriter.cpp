// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Thibaut Gridel <tgridel@free.fr>

#include "KmlPolyStyleTagWriter.h"

#include "GeoDataPolyStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLineStyle(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataPolyStyleType),
                                                                           QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                               new KmlPolyStyleTagWriter);

KmlPolyStyleTagWriter::KmlPolyStyleTagWriter()
    : KmlColorStyleTagWriter(QString::fromLatin1(kml::kmlTag_PolyStyle))
{
}

bool KmlPolyStyleTagWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    const auto style = static_cast<const GeoDataPolyStyle *>(node);

    if (!style->fill()) {
        writer.writeElement(QStringLiteral("fill"), QStringLiteral("0"));
    }
    if (!style->outline()) {
        writer.writeElement(QStringLiteral("outline"), QStringLiteral("0"));
    }

    return true;
}

bool KmlPolyStyleTagWriter::isEmpty(const GeoNode *node) const
{
    const auto style = static_cast<const GeoDataPolyStyle *>(node);
    return style->fill() && style->outline();
}

}
