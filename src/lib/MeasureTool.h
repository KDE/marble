//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// MeasureTool enables Marble to set and display measure points
//

#ifndef MARBLE_MEASURETOOL_H
#define MARBLE_MEASURETOOL_H


#include <QtGui/QFont>
#include <QtGui/QPen>

#include <GeoDataLineString.h>

#include "global.h"

namespace Marble
{

class MarbleModel;
class GeoPainter;
class ViewportParams;

class MeasureTool : public QObject
{
    Q_OBJECT

 public:
    explicit MeasureTool( MarbleModel *model, QObject *parent = 0 );

    void  paint( GeoPainter *painter, ViewportParams *viewport,
                 bool antialiasing );

 Q_SIGNALS:
    void  numberOfMeasurePointsChanged( int newNumber );

 public Q_SLOTS:
    void  addMeasurePoint( qreal lon, qreal lat );
    void  removeLastMeasurePoint();
    void  removeMeasurePoints();

 private:
    void  drawMeasurePoints( GeoPainter *painter, ViewportParams *viewport );
    void  drawMark( GeoPainter* painter, int x, int y );
    void  drawTotalDistanceLabel( GeoPainter *painter,
                                  qreal totalDistance );

 private:
    Q_DISABLE_COPY( MeasureTool )

    // The line strings in the distance path.
    GeoDataLineString m_measureLineString;

    MarbleModel *m_model;

    QFont   m_font_regular;
    int     m_fontascent;

    QPen    m_pen;
};

}

#endif
