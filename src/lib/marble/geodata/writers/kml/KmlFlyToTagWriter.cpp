
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlFlyToTagWriter.h"

#include "GeoDataAbstractView.h"
#include "GeoDataCamera.h"
#include "GeoDataFlyTo.h"
#include "GeoDataLookAt.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerFlyTo(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataFlyToType),
                                                                       QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                           new KmlFlyToTagWriter);

bool KmlFlyToTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto flyTo = static_cast<const GeoDataFlyTo *>(node);
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_FlyTo));
    writer.writeElement(QString::fromLatin1(kml::kmlTag_nameSpaceGx22), QString::fromLatin1(kml::kmlTag_duration), QString::number(flyTo->duration()));
    if (flyTo->flyToMode() == GeoDataFlyTo::Smooth) {
        // two values, smooth and bounce, bounce is default and can hence be omitted
        writer.writeElement(QString::fromLatin1(kml::kmlTag_nameSpaceGx22), QString::fromLatin1(kml::kmlTag_flyToMode), QStringLiteral("smooth"));
    }
    if (flyTo->view()) {
        auto const *lookAt = dynamic_cast<const GeoDataLookAt *>(flyTo->view());
        if (lookAt) {
            writeElement(lookAt, writer);
        }
        auto const *camera = dynamic_cast<const GeoDataCamera *>(flyTo->view());
        if (camera) {
            writeElement(camera, writer);
        }
    }
    writer.writeEndElement();
    return true;
}

}
