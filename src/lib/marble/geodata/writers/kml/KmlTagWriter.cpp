// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#include "KmlTagWriter.h"

#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerKml(GeoTagWriter::QualifiedName({}, QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)), new KmlTagWriter());

bool KmlTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    Q_UNUSED(node);

    writer.writeDefaultNamespace(QString::fromUtf8(kml::kmlTag_nameSpaceOgc22));
    writer.writeNamespace(QString::fromUtf8(kml::kmlTag_nameSpaceGx22), "gx");
    writer.writeStartElement("kml");

    // Do not write an end element for document handlers
    return true;
}

}
