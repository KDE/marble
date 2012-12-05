//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef SCREENOVERLAYGRAPHICSITEM_H
#define SCREENOVERLAYGRAPHICSITEM_H

#include "GeoDataScreenOverlay.h"
#include "GeoDataVec2.h"
#include "GeoPainter.h"
#include "ScreenGraphicsItem.h"
#include "marble_export.h"

#include <QtGui/QImage>

namespace Marble {

class MARBLE_EXPORT ScreenOverlayGraphicsItem : public ScreenGraphicsItem
{
public:
    ScreenOverlayGraphicsItem( const GeoDataScreenOverlay *screenOverlay );

    void setProjection( const ViewportParams *viewport );

protected:
    virtual void paint( QPainter* painter );

private:
    qreal pixelValue( GeoDataVec2::Unit unit, qreal screenSize, qreal imageSize, qreal value ) const;

    const GeoDataScreenOverlay *m_screenOverlay;

    QImage m_image;
};

}

#endif
