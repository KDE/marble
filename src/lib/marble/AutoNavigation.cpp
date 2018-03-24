//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "AutoNavigation.h"

#include "GeoDataCoordinates.h"
#include "PositionTracking.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "ViewportParams.h"
#include "MarbleGlobal.h"
#include "RoutingManager.h"
#include "RoutingModel.h"
#include "Route.h"

#include <QWidget>
#include <QRect>
#include <QPointF>
#include <QTimer>
#include <cmath>

namespace Marble {

class Q_DECL_HIDDEN AutoNavigation::Private
{
public:

    AutoNavigation *const m_parent;
    const MarbleModel *const m_model;
    const ViewportParams *const m_viewport;
    const PositionTracking *const m_tracking;
    AutoNavigation::CenterMode m_recenterMode;
    bool m_adjustZoom;
    QTimer m_lastWidgetInteraction;
    bool m_selfInteraction;

    /** Constructor */
    Private( MarbleModel *model, const ViewportParams *viewport, AutoNavigation *parent );

    /**
     * @brief To center on when reaching custom defined border
     * @param position current gps location
     * @param speed optional speed argument
     */
     void moveOnBorderToCenter( const GeoDataCoordinates &position, qreal speed );

    /**
     * For calculating intersection point of projected LineString from
     * current gps location with the map border
     * @param position current gps location
     */
     GeoDataCoordinates findIntersection( qreal currentX, qreal currentY ) const;

    /**
     * @brief Adjust the zoom value of the map
     * @param currentPosition current location of the gps device
     */
     void adjustZoom( const GeoDataCoordinates &currentPosition, qreal speed );

     /**
       * Center the widget on the given position unless recentering is currently inhibited
       */
     void centerOn( const GeoDataCoordinates &position );
};

AutoNavigation::Private::Private( MarbleModel *model, const ViewportParams *viewport, AutoNavigation *parent ) :
        m_parent( parent ),
        m_model( model ),
        m_viewport( viewport ),
        m_tracking( model->positionTracking() ),
        m_recenterMode( AutoNavigation::DontRecenter ),
        m_adjustZoom( 0 ),
        m_selfInteraction( false )
{
    m_lastWidgetInteraction.setInterval( 10 * 1000 );
    m_lastWidgetInteraction.setSingleShot( true );
}

void AutoNavigation::Private::moveOnBorderToCenter( const GeoDataCoordinates &position, qreal )
{
    qreal x = 0.0;
    qreal y = 0.0;
    //recenter if initially the gps location is not visible on the screen
    if(!( m_viewport->screenCoordinates( position, x, y ) ) ) {
         centerOn( position );
    }
    qreal centerLon = m_viewport->centerLongitude();
    qreal centerLat = m_viewport->centerLatitude();

    qreal centerX = 0.0;
    qreal centerY = 0.0;

    m_viewport->screenCoordinates( centerLon, centerLat, centerX, centerY );

    const qreal borderRatio = 0.25;
    //defining the default border distance from map center
    int shiftX = qRound( centerX * borderRatio );
    int shiftY = qRound( centerY * borderRatio );

    QRect recenterBorderBound;
    recenterBorderBound.setCoords( centerX-shiftX, centerY-shiftY, centerX+shiftX,  centerY+shiftY );

    if( !recenterBorderBound.contains( x,y ) ) {
        centerOn( position );
    }
}

GeoDataCoordinates AutoNavigation::Private::findIntersection( qreal currentX, qreal currentY ) const
{
    qreal direction = m_tracking->direction();
    if ( direction >= 360 ) {
        direction = fmod( direction,360.0 );
    }

    const qreal width = m_viewport->width();
    const qreal height = m_viewport->height();

    QPointF intercept;
    QPointF destinationHorizontal;
    QPointF destinationVertical;
    QPointF destination;

    bool crossHorizontal =  false;
    bool crossVertical = false;

    //calculation of intersection point
    if( 0 < direction && direction < 90 ) {
        const qreal angle = direction * DEG2RAD;

        //Intersection with line x = width
        intercept.setX( width - currentX );
        intercept.setY( intercept.x() / tan( angle ) );
        destinationVertical.setX( width );
        destinationVertical.setY( currentY-intercept.y() );

        //Intersection with line y = 0
        intercept.setY( currentY );
        intercept.setX( intercept.y() * tan( angle ) );
        destinationHorizontal.setX( currentX + intercept.x() );
        destinationHorizontal.setY( 0 );

        if ( destinationVertical.y() < 0 ) {
            crossHorizontal = true;
        }
        else if( destinationHorizontal.x() > width ) {
            crossVertical = true;
        }

    }
    else if( 270 < direction && direction < 360 ) {
        const qreal angle = (direction - 270) * DEG2RAD;

        //Intersection with line y = 0
        intercept.setY( currentY );
        intercept.setX( intercept.y() / tan( angle ) );
        destinationHorizontal.setX( currentX - intercept.x() );
        destinationHorizontal.setY( 0 );

        //Intersection with line x = 0
        intercept.setX( currentX );
        intercept.setY( intercept.x() * tan( angle ) );
        destinationVertical.setY( currentY - intercept.y() );
        destinationVertical.setX( 0 );

        if( destinationHorizontal.x() > width ) {
            crossVertical = true;
        }
        else if( destinationVertical.y() < 0 ) {
            crossHorizontal = true;
        }

    }
    else if( 180 < direction && direction < 270  ) {
        const qreal angle = (direction - 180) * DEG2RAD;

        //Intersection with line x = 0
        intercept.setX( currentX );
        intercept.setY( intercept.x() / tan( angle ) );
        destinationVertical.setY( currentY + intercept.y() );
        destinationVertical.setX( 0 );

        //Intersection with line y = height
        intercept.setY( currentY );
        intercept.setX( intercept.y() * tan( angle ) );
        destinationHorizontal.setX( currentX - intercept.x() );
        destinationHorizontal.setY( height );

        if ( destinationVertical.y() > height ) {
            crossHorizontal = true;
        }
        else if ( destinationHorizontal.x() < 0 ) {
            crossVertical = true;
        }

    }
    else if( 90 < direction && direction < 180  ) {
        const qreal angle = (direction - 90) * DEG2RAD;

        //Intersection with line y = height
        intercept.setY( height - currentY );
        intercept.setX( intercept.y() / tan( angle ) );
        destinationHorizontal.setX( currentX + intercept.x() );
        destinationHorizontal.setY( height );

        //Intersection with line x = width
        intercept.setX( width - currentX );
        intercept.setY( intercept.x() * tan( angle ) );
        destinationVertical.setX( width );
        destinationVertical.setY( currentY + intercept.y() );

        if ( destinationHorizontal.x() > width ) {
            crossVertical = true;
        }
        else if( destinationVertical.y() > height ) {
            crossHorizontal = true;
        }

    }
    else if( direction == 0 ) {
        destinationHorizontal.setX( currentX );
        destinationHorizontal.setY( 0 );
        crossHorizontal = true;
    }
    else if( direction == 90 ) {
        destinationVertical.setX( width );
        destinationVertical.setY( currentY );
        crossVertical = true;
    }
    else if( direction == 180 ) {
        destinationHorizontal.setX( currentX );
        destinationHorizontal.setY( height );
        crossHorizontal = true;
    }
    else if( direction == 270 ) {
        destinationVertical.setX( 0 );
        destinationVertical.setY( currentY );
        crossVertical = true;
    }

    if ( crossHorizontal == true && crossVertical == false ) {
        destination.setX( destinationHorizontal.x() );
        destination.setY( destinationHorizontal.y() );
    }
    else if ( crossVertical == true && crossHorizontal == false ) {
        destination.setX( destinationVertical.x() );
        destination.setY( destinationVertical.y() );
    }

    qreal destinationLon = 0.0;
    qreal destinationLat = 0.0;
    m_viewport->geoCoordinates( destination.x(), destination.y(), destinationLon, destinationLat,
                              GeoDataCoordinates::Radian );
    GeoDataCoordinates destinationCoord( destinationLon, destinationLat, GeoDataCoordinates::Radian );

    return destinationCoord;
}

void AutoNavigation::Private::adjustZoom( const GeoDataCoordinates &currentPosition, qreal speed )
{
    qreal currentX = 0;
    qreal currentY = 0;
    if( !m_viewport->screenCoordinates(currentPosition, currentX, currentY ) ) {
        return;
    }

    const GeoDataCoordinates destination = findIntersection( currentX, currentY );

    const qreal greatCircleDistance = currentPosition.sphericalDistanceTo(destination);
    qreal radius = m_model->planetRadius();
    qreal distance = greatCircleDistance *  radius;

    if( speed != 0 ) {
        // time (in seconds) remaining to reach the border of the map
        qreal  remainingTime = distance / speed;

        // tolerance time limits (in seconds) before auto zooming
        qreal thresholdLow = 15;
        qreal thresholdHigh = 120;

        m_selfInteraction = true;
        if ( remainingTime < thresholdLow ) {
            emit m_parent->zoomOut( Instant );
        }
        else if ( remainingTime > thresholdHigh ) {
            emit m_parent->zoomIn( Instant );
        }
        m_selfInteraction = false;
    }
}

void AutoNavigation::Private::centerOn( const GeoDataCoordinates &position )
{
    m_selfInteraction = true;
    RoutingManager const * routingManager = m_model->routingManager();
    RoutingModel const * routingModel = routingManager->routingModel();
    if (!routingManager->guidanceModeEnabled() || routingModel->deviatedFromRoute()){
        emit m_parent->centerOn( position, false );
    } else {
        GeoDataCoordinates positionOnRoute = routingModel->route().positionOnRoute();
        emit m_parent->centerOn( positionOnRoute, false );
    }
    m_selfInteraction = false;
}

AutoNavigation::AutoNavigation( MarbleModel *model, const ViewportParams *viewport, QObject *parent ) :
    QObject( parent ),
    d( new AutoNavigation::Private( model, viewport, this ) )
{
    connect( d->m_tracking, SIGNAL(gpsLocation(GeoDataCoordinates,qreal)),
                this, SLOT(adjust(GeoDataCoordinates,qreal)) );
}

AutoNavigation::~AutoNavigation()
{
    delete d;
}

void AutoNavigation::adjust( const GeoDataCoordinates &position, qreal speed )
{
    if ( d->m_lastWidgetInteraction.isActive() ) {
        return;
    }

    switch( d->m_recenterMode ) {
    case DontRecenter:
        /* nothing to do */
        break;
    case AlwaysRecenter:
        d->centerOn( position );
        break;
    case RecenterOnBorder:
        d->moveOnBorderToCenter( position, speed );
        break;
    }

    if ( d->m_adjustZoom ) {
        switch( d->m_recenterMode ) {
        case DontRecenter:
            /* nothing to do */
            break;
        case AlwaysRecenter:
        case RecenterOnBorder: // fallthrough
            d->adjustZoom( position, speed );
            break;
        }
    }
}

void AutoNavigation::setAutoZoom( bool autoZoom )
{
    d->m_adjustZoom = autoZoom;
    emit autoZoomToggled( autoZoom );
}

void AutoNavigation::setRecenter( CenterMode recenterMode )
{
    d->m_recenterMode = recenterMode;
    emit recenterModeChanged( recenterMode );
}

void AutoNavigation::inhibitAutoAdjustments()
{
    if ( !d->m_selfInteraction ) {
        d->m_lastWidgetInteraction.start();
    }
}

AutoNavigation::CenterMode AutoNavigation::recenterMode() const
{
    return d->m_recenterMode;
}

bool AutoNavigation::autoZoom() const
{
    return d->m_adjustZoom;
}

} // namespace Marble

#include "moc_AutoNavigation.cpp"
