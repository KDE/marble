// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#include "KmlDataTagWriter.h"

#include "GeoDataData.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerData(GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataDataType, kml::kmlTag_nameSpaceOgc22), new KmlDataTagWriter());

bool KmlDataTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const GeoDataData *data = static_cast<const GeoDataData *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Data));
    writer.writeAttribute("name", data->name());
    writer.writeOptionalElement(kml::kmlTag_displayName, data->displayName());
    writer.writeElement("value", data->value().toString());
    writer.writeEndElement();

    return true;
}

}
