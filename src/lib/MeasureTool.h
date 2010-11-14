//
// This file is part of the Marble Virtual Globe.
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
#include "LayerInterface.h"

namespace Marble
{

class MarbleModel;
class GeoPainter;
class ViewportParams;

class MeasureTool : public QObject, public LayerInterface
{
    Q_OBJECT

 public:
    explicit MeasureTool( MarbleModel *model, QObject *parent = 0 );

    virtual QStringList renderPosition() const { return QStringList(); }

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

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
