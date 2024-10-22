// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#include "KmlDocumentTagWriter.h"

#include "GeoDataDocument.h"
#include "GeoDataSchema.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDocument.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

#include "GeoDataTypes.h"

#include <QList>

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataDocumentType),
                                                                          QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                              new KmlDocumentTagWriter());

KmlDocumentTagWriter::KmlDocumentTagWriter()
    : KmlFeatureTagWriter(QString::fromLatin1(kml::kmlTag_Document))
{
    // nothing to do
}

bool KmlDocumentTagWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    const auto document = static_cast<const GeoDataDocument *>(node);

    for (const GeoDataStyle::ConstPtr &style : document->styles()) {
        writeElement(style.data(), writer);
    }
    for (const GeoDataStyleMap &map : document->styleMaps()) {
        writeElement(&map, writer);
    }
    for (const GeoDataSchema &schema : document->schemas()) {
        writeElement(&schema, writer);
    }

    QList<GeoDataFeature *>::ConstIterator it = document->constBegin();
    QList<GeoDataFeature *>::ConstIterator const end = document->constEnd();

    for (; it != end; ++it) {
        writeElement(*it, writer);
    }

    return true;
}

}
