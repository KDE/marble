//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_GEOPHOTOGRAPHICSITEM_H
#define MARBLE_GEOPHOTOGRAPHICSITEM_H

#include "GeoDataPoint.h"
#include "GeoGraphicsItem.h"
#include "marble_export.h"

#include <QImage>

namespace Marble
{

class MARBLE_EXPORT GeoPhotoGraphicsItem : public GeoGraphicsItem
{
public:
    explicit GeoPhotoGraphicsItem( const GeoDataFeature *feature );

    void setPoint( const GeoDataPoint& point );

    GeoDataPoint point() const;

    virtual void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer);

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

protected:
    GeoDataPoint m_point;

    QImage m_photo;
};

}

#endif
