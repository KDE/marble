//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
//


#include "AdjustNavigation.h"

#include "MarbleWidget.h"
#include "GeoDataCoordinates.h"
#include "gps/PositionTracking.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "ViewportParams.h"
#include "MarbleMath.h"
#include "global.h"

#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <QtCore/QRect>
#include <QtCore/QPointF>
#include <math.h>

using namespace Marble;

AdjustNavigation::AdjustNavigation( MarbleWidget *widget, QObject *parent )
    :QObject( parent ),
     m_widget( widget ),
     m_tracking( 0 ),
     m_gpsSpeed( 0 ),
     m_gpsDirection( 0 ),
     m_recenterMode( 0 ),
     m_adjustZoom( 0 )
{
}

void AdjustNavigation::adjust( GeoDataCoordinates position, qreal speed )
{
    if( !m_widget) {
        return;
    }

    m_gpsDirection = m_tracking->direction();
    m_gpsSpeed = speed;
    if( m_recenterMode && m_adjustZoom ) {
        if( m_recenterMode == AlwaysRecenter ) {
            m_widget->centerOn( position, false );
        }
        else if( m_recenterMode == RecenterOnBorder ) {
            moveOnBorderToCenter( position, speed );
        }
        findIntersection( position );
    }
    else if( m_recenterMode == AlwaysRecenter ) {
        m_widget->centerOn( position, false );
    }
    else if( m_recenterMode == RecenterOnBorder ) {
        moveOnBorderToCenter( position, speed );
    }
    else if( m_adjustZoom ) {
        findIntersection( position );
    }
}

void AdjustNavigation::setRecenter( int recenterMode )
{
    m_recenterMode = recenterMode;
    emit recenterModeChanged( recenterMode );

    PositionTracking * tracking = m_widget->model()->positionTracking();
    if( recenterMode ) {
        m_tracking = tracking;
        QObject::connect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                  this, SLOT( adjust( GeoDataCoordinates, qreal ) ), Qt::UniqueConnection );
    }
    else {
        QObject::disconnect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                  this, SLOT( adjust( GeoDataCoordinates, qreal ) ) );
    }
}

void AdjustNavigation::moveOnBorderToCenter( GeoDataCoordinates position, qreal )
{
    qreal lon = 0.0;
    qreal lat = 0.0;

    position.geoCoordinates( lon, lat, GeoDataCoordinates::Degree );

    qreal x = 0.0;
    qreal y = 0.0;
    //recenter if initially the gps location is not visible on the screen
    if(!( m_widget->screenCoordinates( lon, lat, x, y ) ) ) {
         m_widget->centerOn( position, false );
    }
    qreal centerLon = m_widget->centerLongitude();
    qreal centerLat = m_widget->centerLatitude();

    qreal centerX = 0.0;
    qreal centerY = 0.0;

    m_widget->screenCoordinates( centerLon, centerLat, centerX, centerY );

    const qreal borderRatio = 0.65;
    //defining the default border distance from map center
    int shiftX = qRound( centerX * borderRatio );
    int shiftY = qRound( centerY * borderRatio );

    QRect recenterBorderBound;
    recenterBorderBound.setCoords( centerX-shiftX, centerY-shiftY, centerX+shiftX,  centerY+shiftY );

    if( !recenterBorderBound.contains( x,y ) ) {
        m_widget->centerOn( position, false );
    }
}

void AdjustNavigation::setAutoZoom( bool autoZoom )
{
    m_adjustZoom = autoZoom;
    emit autoZoomToggled( autoZoom );

    PositionTracking * tracking = m_widget->model()->positionTracking();
    if( autoZoom ) {
        m_tracking = tracking;
        QObject::connect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                this, SLOT( adjust( GeoDataCoordinates, qreal ) ), Qt::UniqueConnection );
    }
    else {
        QObject::disconnect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                 this, SLOT( adjust( GeoDataCoordinates, qreal ) ) );
    }
}

void AdjustNavigation::findIntersection( GeoDataCoordinates position )
{
    qreal track = m_gpsDirection;

    if ( track >= 360 ) {
        track = fmod( track,360.0 );
    }

    qreal lon = 0;
    qreal lat = 0;
    position.geoCoordinates( lon, lat, GeoDataCoordinates::Degree );

    qreal currentX = 0;
    qreal currentY = 0;

    if( !m_widget->screenCoordinates( lon, lat, currentX, currentY ) ) {
        m_widget->centerOn( position, false );
    }
    else {
        ViewportParams const * const viewparams = m_widget->viewport();

        qreal width = viewparams->width();
        qreal height = viewparams->height();

        QPointF intercept;
        QPointF destinationHorizontal;
        QPointF destinationVertical;
        QPointF destination;

        qreal angle = 0.0;
        bool crossHorizontal =  false;
        bool crossVertical = false;

        //calculation of intersection point
        if( track > 0 && track < 90 ) {
            angle = track;

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
        else if( track > 270 && track < 360 ) {
            angle = track - 270;

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
        else if( track > 180 && track < 270  ) {
            angle = track - 180;

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
        else if( track > 90 && track < 180  ) {
            angle = track - 90;

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
        else {
            if( track == 0 ) {
                destinationHorizontal.setX( currentX );
                destinationHorizontal.setY( 0 );
                crossHorizontal = true;
            }
            else if( track == 90 ) {
                destinationVertical.setX( width );
                destinationVertical.setY( currentY );
                crossVertical = true;
            }
            else if( track == 190 ) {
                destinationHorizontal.setX( currentX );
                destinationHorizontal.setY( height );
                crossHorizontal = true;
            }
            else if( track == 270 ) {
                destinationVertical.setX( 0 );
                destinationVertical.setY( currentY );
                crossVertical = true;
            }
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
        m_widget->geoCoordinates( destination.x(), destination.y(), destinationLon, destinationLat,
                                  GeoDataCoordinates::Radian );
        GeoDataCoordinates destinationCoord( destinationLon, destinationLat, GeoDataCoordinates::Radian );
        adjustZoom( position, destinationCoord );
    }
}

void AdjustNavigation::adjustZoom( GeoDataCoordinates currentPosition, GeoDataCoordinates destination )
{
    qreal greatCircleDistance = distanceSphere( currentPosition, destination );
    qreal radius = m_widget->model()->planetRadius();
    qreal distance = greatCircleDistance *  radius;

    if( m_gpsSpeed != 0 ) {
        //time(in minutes) remaining to reach the border of the map
        qreal  remainingTime = ( distance / m_gpsSpeed ) * SEC2MIN;

        //tolerance time limits( in minutes ) before auto zooming
        qreal thresholdVeryLow = 0.5;
        qreal thresholdLow = 2.0 * thresholdVeryLow;
        qreal thresholdHigh = 12.0 * thresholdLow;
        qreal thresholdVeryHigh = 3.0 * thresholdHigh;
        qreal thresholdExtreme = 4.0 * thresholdVeryHigh;

        int zoom = 0;

        if ( remainingTime <= thresholdVeryLow ) {
            zoom = 3100;
            m_widget->zoomView( zoom );
        }

        if ( remainingTime > thresholdVeryLow && remainingTime < thresholdLow ) {
            m_widget->zoomOut();
        }

        if ( remainingTime >= thresholdHigh && remainingTime < thresholdVeryHigh ) {
            m_widget->zoomIn();
        }

        if ( remainingTime >= thresholdVeryHigh && remainingTime < thresholdExtreme ) {
            int zoomStep = 55;
            m_widget->zoomViewBy( zoomStep );
        }

        if ( remainingTime >= thresholdExtreme ) {
             zoom = 3000;
             m_widget->zoomView( zoom );
        }
    }
}

#include "AdjustNavigation.moc"
