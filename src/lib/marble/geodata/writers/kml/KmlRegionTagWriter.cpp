// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlRegionTagWriter.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataLod.h"
#include "GeoDataRegion.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerRegion(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataRegionType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlRegionTagWriter);

bool KmlRegionTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto region = static_cast<const GeoDataRegion *>(node);
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Region));
    KmlObjectTagWriter::writeIdentifiers(writer, region);
    writeElement(&region->latLonAltBox(), writer);
    writeElement(&region->lod(), writer);
    writer.writeEndElement();
    return true;
}

}
