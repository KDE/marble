//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
//

#ifndef MARBLE_POSITIONTRACKING_H
#define MARBLE_POSITIONTRACKING_H

#include "PositionProviderPlugin.h"

#include <QtGui/QRegion>
#include <QtGui/QPolygonF>
#include <QtCore/QObject>
#include <QtCore/QTemporaryFile>
#include <QtNetwork/QHttp>

namespace Marble
{

class GeoDataDocument;
class ClipPainter;
class ViewParams;
class GeoDataCoordinates;
class PluginManager;
class MarbleGeometryModel;

class PositionTracking : public QObject 
{
    Q_OBJECT

public:

    explicit PositionTracking( MarbleGeometryModel *geometryModel,
                          QObject *parent = 0 );
    ~PositionTracking();

    /**
     * @brief update the provider position
     * 
     * this function is designed to be polled at a regular interval so
     * that the provider object can update its position and construct the 
     * new polygon that needs to be painted to screen. 
     * 
     * @param canvasSize current size of view
     * @param radius current radius of globe
     * @param invRotAxis current Quaternion representation of globe.
     * @param reg the region on screen that needs to be re-painted
     *            following this update
     * @return @c true the region of the screen needs to be repainted
     *         @c false the gps object was not updated so no repaint
     *                  necessary.
     */
    bool update(const QSize &canvasSize, ViewParams *viewParams,
                QRegion &reg);

    void construct ( const QSize &canvasSize,
                     ViewParams *viewParams );

    void draw( ClipPainter *painter,
                const QSize &canvasSize, 
                ViewParams *viewParams );

    /**
      * Change the position provider to use. You can provide 0 to disable
      * position tracking. Ownership of the provided plugin is taken.
      */
    void setPositionProviderPlugin( PositionProviderPlugin* plugin );

    QString error() const;

Q_SIGNALS:
    void  gpsLocation( GeoDataCoordinates, qreal );

    void statusChanged( PositionProviderStatus status );

public slots:
    void setPosition( GeoDataCoordinates position,
                          GeoDataAccuracy accuracy );

 private:
    void updateSpeed( GeoDataCoordinates& previous, GeoDataCoordinates next );

    qreal               m_speed;
    //used to draw the arrow in gps tracking
    QPointF             m_relativeTip;
    QPointF             m_relativeLeft;
    QPointF             m_relativeRight;
    QPolygonF           m_currentDraw;
    QPolygonF           m_previousDraw;

    GeoDataDocument     *m_document;
    MarbleGeometryModel *m_geometryModel;

    GeoDataCoordinates  m_gpsCurrentPosition;
    GeoDataCoordinates  m_gpsPreviousPosition;

    PositionProviderPlugin* m_positionProvider;
};

}



#endif
