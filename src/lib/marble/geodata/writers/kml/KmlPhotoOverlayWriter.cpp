//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyatf@gmail.com>
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlPhotoOverlayWriter.h"

#include "GeoDataPhotoOverlay.h"
#include "GeoDataTypes.h"
#include "GeoDataImagePyramid.h"
#include "GeoDataPoint.h"
#include "GeoDataViewVolume.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataPhotoOverlayType,
				 kml::kmlTag_nameSpaceOgc22 ),
    new KmlPhotoOverlayWriter );

KmlPhotoOverlayWriter::KmlPhotoOverlayWriter() : KmlOverlayTagWriter( kml::kmlTag_PhotoOverlay )
{
    // nothing to do
}

bool KmlPhotoOverlayWriter::writeMid( const GeoNode *node, GeoWriter &writer ) const
{
    KmlOverlayTagWriter::writeMid( node, writer );

    const GeoDataPhotoOverlay *photo_overlay =
        static_cast<const GeoDataPhotoOverlay*>( node );

    // rotation
    QString const rotation = QString::number( photo_overlay->rotation(), 'f', 3 );
    writer.writeOptionalElement( kml::kmlTag_rotation, rotation, "0.000" );

    // ViewVolume
    writer.writeStartElement( kml::kmlTag_ViewVolume );
    writer.writeOptionalElement<qreal>( kml::kmlTag_leftFov, photo_overlay->viewVolume().leftFov(), 0 );
    writer.writeOptionalElement<qreal>( kml::kmlTag_rightFov, photo_overlay->viewVolume().rightFov(), 0 );
    writer.writeOptionalElement<qreal>( kml::kmlTag_bottomFov, photo_overlay->viewVolume().bottomFov(), 0 );
    writer.writeOptionalElement<qreal>( kml::kmlTag_topFov, photo_overlay->viewVolume().topFov(), 0 );
    writer.writeOptionalElement<qreal>( kml::kmlTag_near, photo_overlay->viewVolume().near(), 0 );
    writer.writeEndElement();

    // ImagePyramid
    writer.writeStartElement( kml::kmlTag_ImagePyramid );
    writer.writeOptionalElement<int>( kml::kmlTag_tileSize, photo_overlay->imagePyramid().tileSize(), 256 );
    writer.writeOptionalElement<int>( kml::kmlTag_maxWidth, photo_overlay->imagePyramid().maxWidth() );
    writer.writeOptionalElement<int>( kml::kmlTag_maxHeight, photo_overlay->imagePyramid().maxHeight() );

    switch ( photo_overlay->imagePyramid().gridOrigin() )
    {
    case GeoDataImagePyramid::LowerLeft:
        writer.writeElement( kml::kmlTag_gridOrigin, "lowerLeft" );
        break;
    case GeoDataImagePyramid::UpperLeft:
        writer.writeElement( kml::kmlTag_gridOrigin, "upperLeft" );
        break;
    }
    writer.writeEndElement();

    // Point
    writeElement( &photo_overlay->point(), writer );

    // shape
    switch ( photo_overlay->shape() )
    {
    case GeoDataPhotoOverlay::Rectangle:
        break;
    case GeoDataPhotoOverlay::Cylinder:
        writer.writeElement( kml::kmlTag_shape, "cylinder" );
        break;
    case GeoDataPhotoOverlay::Sphere:
        writer.writeElement( kml::kmlTag_shape, "sphere" );
        break;
    }

    return true;
}

}
