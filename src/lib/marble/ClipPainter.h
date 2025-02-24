// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2009 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

#ifndef MARBLE_CLIPPAINTER_H
#define MARBLE_CLIPPAINTER_H

#include "MarbleGlobal.h"
#include "marble_export.h"
#include <QPainter>

class QPaintDevice;
class QPolygonF;
class QPointF;

namespace Marble
{
/**
 * @short A QPainter that does viewport clipping for polygons
 *
 * This class introduces fast polygon/polyline clipping for QPainter
 * to increase the performance.
 * Clipping is accomplished using an algorithm (by Torsten Rahn) that
 * processes each polyline once.
 * To keep things fast each possible scenario of two subsequent
 * points is implemented case by case in a specialized handler which
 * creates interpolated points and helper points.
 */

// The reason for this class is a terrible bug in some versions of the
// X Server.  Suppose the widget size is, say, 1000 x 1000 and we have
// a high zoom so that we want to draw a vector from (-100000,
// -100000) to (100000, 100000).  Then the X server will create a
// bitmap that is at least 100000 x 100000 and in the process eat all
// available memory.
//
// So we introduce the ClipPainter that clips all polylines and polygons
// to the area that is actually visible in the viewport.
//
// @internal

class ClipPainterPrivate;
class MARBLE_EXPORT ClipPainter : public QPainter
{
public:
    ClipPainter();
    ClipPainter(QPaintDevice *, bool);

    ~ClipPainter();

    void setScreenClip(bool enable);
    bool hasScreenClip() const;

    void drawPolygon(const QPolygonF &, Qt::FillRule fillRule = Qt::OddEvenFill);

    void drawPolyline(const QPolygonF &);
    void drawPolyline(const QPolygonF &, QList<QPointF> &labelNodes, LabelPositionFlags labelPositionFlag = LineCenter);

    void labelPosition(const QPolygonF &polygon, QList<QPointF> &labelNodes, LabelPositionFlags labelPositionFlags) const;

    void setPen(const QColor &);
    void setPen(const QPen &pen);
    void setPen(Qt::PenStyle style);
    void setBrush(const QBrush &brush);

    void setDebugPolygonsLevel(int);
    void setDebugBatchRender(bool);

    //	void clearNodeCount(){ m_debugNodeCount = 0; }
    //	int nodeCount(){ return m_debugNodeCount; }

private:
    ClipPainterPrivate *const d;
};

}

#endif
