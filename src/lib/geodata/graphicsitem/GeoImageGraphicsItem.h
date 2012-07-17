//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_GEOIMAGEGRAPHICSITEM_H
#define MARBLE_GEOIMAGEGRAPHICSITEM_H

#include "GeoDataLatLonBox.h"
#include "GeoGraphicsItem.h"
#include "marble_export.h"

#include <QtGui/QImage>

namespace Marble
{

class MARBLE_EXPORT GeoImageGraphicsItem : public GeoGraphicsItem
{
public:
    void setLatLonBox( const GeoDataLatLonBox& box );

    GeoDataLatLonBox& latLonBox() const;

    void setImage( const QImage &image );

    QImage image() const;

    void setImageFile( const QString &filename );

    QString imageFile() const;

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );

protected:
    QImage m_image;

    QString m_imageFile;
};

}

#endif
