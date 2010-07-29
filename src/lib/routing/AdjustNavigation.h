//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
//

#ifndef ADJUSTNAVIGATION_H
#define ADJUSTNAVIGATION_H

#include "MarbleWidget.h"
#include "GeoDataCoordinates.h"

#include <QtGui/QPixmap>
#include <QtGui/QWidget>

namespace Marble
{

class GeoDataCoordinates;
class MarbleWidget;
class PositionTracking;
class AdjustNavigation : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param widget the marble widget. It cannot be null.
     * @param parent optional parent object
     */
    explicit AdjustNavigation( MarbleWidget *widget, QObject *parent = 0 );

    /**
    * An enum type
    * Represents which recentering method is selected
    */
    enum CenterMode{
            AlwaysRecenter = 1,    /**< Enum Value AlwaysRecenter. Recenter always to the map center */
            RecenterOnBorder = 2   /**< Enum Value RecenterOnBorder. Recenter when reaching map border */
    };

    /**
      * @brief For Auto Centering adjustment of map in Navigation Mode
      * @param recenterMode toggles among the recenteing method chosen
      * @see CenterMode
      */
      void setRecenter( int recenterMode );

    /**
      * @brief For Auto Zooming adjustment of map in Navigation Mode
      * @param activate true to enable auto zooming
      */
      void setAutoZoom( bool activate );

public Q_SLOTS:

    /**
      * @brief For adjusting the gps location (recentering) or map(autozooming)
      * @param position current gps location
      * @param speed of the gps device
      */
      void adjust( GeoDataCoordinates position, qreal speed );

private:

    /**
    * @brief To center on when reaching custom defined border
    * @param position current gps location
    * @param speed optional speed argument
    */
    void moveOnBorderToCenter( GeoDataCoordinates position, qreal speed );

    /**
    * For calculating intersection point of projected LineString from
    * current gps location with the map border
    * @param position current gps location
    */
    void findIntersection( GeoDataCoordinates position );

    /**
    * @brief Adjust the zoom value of the map
    * @param currentPosition current location of the gps device
    * @param destination geoCoordinates of the point on the screen border where the gps device
    * would reach if allowed to move in that direction
    */
    void adjustZoom( GeoDataCoordinates currentPosition , GeoDataCoordinates destination );

    MarbleWidget        *m_widget;
    PositionTracking    *m_tracking;
    qreal                m_gpsSpeed;
    qreal                m_gpsDirection;
    int                  m_recenterMode;
    bool                 m_adjustZoom;

};
} //namespace marble

#endif // ADJUSTNAVIGATION_H
