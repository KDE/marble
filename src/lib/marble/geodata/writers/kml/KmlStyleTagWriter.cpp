// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlStyleTagWriter.h"

#include "GeoDataBalloonStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataListStyle.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerStyle(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataStyleType),
                                                                       QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                           new KmlStyleTagWriter);

bool KmlStyleTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto style = static_cast<const GeoDataStyle *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Style));
    KmlObjectTagWriter::writeIdentifiers(writer, style);

    writeElement(&style->iconStyle(), writer);
    writeElement(&style->labelStyle(), writer);
    writeElement(&style->lineStyle(), writer);
    writeElement(&style->polyStyle(), writer);
    writeElement(&style->balloonStyle(), writer);
    writeElement(&style->listStyle(), writer);

    writer.writeEndElement();

    return true;
}

}
