//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef SCREENOVERLAYGRAPHICSITEM_H
#define SCREENOVERLAYGRAPHICSITEM_H

#include "GeoDataVec2.h"
#include "ScreenGraphicsItem.h"
#include "marble_export.h"

#include <QPixmap>

namespace Marble {

class GeoDataScreenOverlay;

class MARBLE_EXPORT ScreenOverlayGraphicsItem : public ScreenGraphicsItem
{
public:
    explicit ScreenOverlayGraphicsItem( const GeoDataScreenOverlay *screenOverlay );

    const GeoDataScreenOverlay* screenOverlay() const;

    void setProjection( const ViewportParams *viewport );

protected:
    virtual void paint( QPainter* painter );

private:
    static qreal pixelValue( GeoDataVec2::Unit unit, qreal screenSize, qreal imageSize, qreal value );

    const GeoDataScreenOverlay *m_screenOverlay;

    QPixmap m_pixmap;
};

}

#endif
