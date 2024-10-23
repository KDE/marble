// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyatf@gmail.com>
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlPhotoOverlayWriter.h"

#include "GeoDataImagePyramid.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataPoint.h"
#include "GeoDataTypes.h"
#include "GeoDataViewVolume.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataPhotoOverlayType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlPhotoOverlayWriter);

KmlPhotoOverlayWriter::KmlPhotoOverlayWriter()
    : KmlOverlayTagWriter(QString::fromLatin1(kml::kmlTag_PhotoOverlay))
{
    // nothing to do
}

bool KmlPhotoOverlayWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    KmlOverlayTagWriter::writeMid(node, writer);

    const auto photo_overlay = static_cast<const GeoDataPhotoOverlay *>(node);

    // rotation
    QString const rotation = QString::number(photo_overlay->rotation(), 'f', 3);
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_rotation), rotation, QStringLiteral("0.000"));

    // ViewVolume
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_ViewVolume));
    writer.writeOptionalElement<qreal>(QString::fromLatin1(kml::kmlTag_leftFov), photo_overlay->viewVolume().leftFov(), 0);
    writer.writeOptionalElement<qreal>(QString::fromLatin1(kml::kmlTag_rightFov), photo_overlay->viewVolume().rightFov(), 0);
    writer.writeOptionalElement<qreal>(QString::fromLatin1(kml::kmlTag_bottomFov), photo_overlay->viewVolume().bottomFov(), 0);
    writer.writeOptionalElement<qreal>(QString::fromLatin1(kml::kmlTag_topFov), photo_overlay->viewVolume().topFov(), 0);
    writer.writeOptionalElement<qreal>(QString::fromLatin1(kml::kmlTag_near), photo_overlay->viewVolume().near(), 0);
    writer.writeEndElement();

    // ImagePyramid
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_ImagePyramid));
    writer.writeOptionalElement<int>(QString::fromLatin1(kml::kmlTag_tileSize), photo_overlay->imagePyramid().tileSize(), 256);
    writer.writeOptionalElement<int>(QString::fromLatin1(kml::kmlTag_maxWidth), photo_overlay->imagePyramid().maxWidth());
    writer.writeOptionalElement<int>(QString::fromLatin1(kml::kmlTag_maxHeight), photo_overlay->imagePyramid().maxHeight());

    switch (photo_overlay->imagePyramid().gridOrigin()) {
    case GeoDataImagePyramid::LowerLeft:
        writer.writeElement(QString::fromLatin1(kml::kmlTag_gridOrigin), QStringLiteral("lowerLeft"));
        break;
    case GeoDataImagePyramid::UpperLeft:
        writer.writeElement(QString::fromLatin1(kml::kmlTag_gridOrigin), QStringLiteral("upperLeft"));
        break;
    }
    writer.writeEndElement();

    // Point
    writeElement(&photo_overlay->point(), writer);

    // shape
    switch (photo_overlay->shape()) {
    case GeoDataPhotoOverlay::Rectangle:
        break;
    case GeoDataPhotoOverlay::Cylinder:
        writer.writeElement(QString::fromLatin1(kml::kmlTag_shape), QStringLiteral("cylinder"));
        break;
    case GeoDataPhotoOverlay::Sphere:
        writer.writeElement(QString::fromLatin1(kml::kmlTag_shape), QStringLiteral("sphere"));
        break;
    }

    return true;
}

}
