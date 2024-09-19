// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef SCREENOVERLAYGRAPHICSITEM_H
#define SCREENOVERLAYGRAPHICSITEM_H

#include "GeoDataVec2.h"
#include "ScreenGraphicsItem.h"
#include "marble_export.h"

#include <QPixmap>

namespace Marble
{

class GeoDataScreenOverlay;

class MARBLE_EXPORT ScreenOverlayGraphicsItem : public ScreenGraphicsItem
{
public:
    explicit ScreenOverlayGraphicsItem(const GeoDataScreenOverlay *screenOverlay);

    const GeoDataScreenOverlay *screenOverlay() const;

    void setProjection(const ViewportParams *viewport) override;

protected:
    void paint(QPainter *painter) override;

private:
    static qreal pixelValue(GeoDataVec2::Unit unit, qreal screenSize, qreal imageSize, qreal value);

    const GeoDataScreenOverlay *m_screenOverlay;

    QPixmap m_pixmap;
};

}

#endif
