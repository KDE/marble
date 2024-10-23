// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlLodTagWriter.h"

#include "GeoDataLod.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLod(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataLodType),
                                                                     QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                         new KmlLodTagWriter);

bool KmlLodTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto lod = static_cast<const GeoDataLod *>(node);
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Lod));
    KmlObjectTagWriter::writeIdentifiers(writer, lod);
    writer.writeTextElement(QString::fromUtf8(kml::kmlTag_minLodPixels), QString::number(lod->minLodPixels()));
    writer.writeTextElement(QString::fromUtf8(kml::kmlTag_maxLodPixels), QString::number(lod->maxLodPixels()));
    writer.writeTextElement(QString::fromUtf8(kml::kmlTag_minFadeExtent), QString::number(lod->minFadeExtent()));
    writer.writeTextElement(QString::fromUtf8(kml::kmlTag_maxFadeExtent), QString::number(lod->maxFadeExtent()));
    writer.writeEndElement();
    return true;
}

}
