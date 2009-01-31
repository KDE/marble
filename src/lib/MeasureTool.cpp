//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>"
// Copyright 2007-2008 Carlos Licea <carlos.licea@kdemail.net>
//


// local
#include "MeasureTool.h"

// posix
#include <cmath>

// Qt
#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtGui/QPainter>

// marble
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "MarbleLocale.h"
#include "AbstractProjection.h"
#include "GeoPainter.h"
#include "Quaternion.h"
#include "ViewportParams.h"

#include "global.h"

using namespace Marble;

MeasureTool::MeasureTool( MarbleModel *model, QObject* parent )
    : QObject( parent ),
      m_model( model )
{
#ifdef Q_OS_MACX
    m_font_regular = QFont( "Sans Serif", 10, 50, false );
#else
    m_font_regular = QFont( "Sans Serif",  8, 50, false );
#endif

    m_fontheight = QFontMetrics( m_font_regular ).height();
    m_fontascent = QFontMetrics( m_font_regular ).ascent();

    m_pen.setColor( QColor( Qt::red ) );
    m_pen.setWidthF( 2.0 );
}


void MeasureTool::paint( GeoPainter *painter, 
                         ViewportParams *viewport,
                         bool antialiasing )
{
    // No way to paint anything if the list is empty.
    if ( m_pMeasurePointList.isEmpty() )
        return;

    // Prepare the painter for the paths.
    painter->setRenderHint( QPainter::Antialiasing, antialiasing );
    painter->setPen( m_pen );

    // Paint the paths and calculate the total length
    qreal  totalDistance = 0.0;
    qreal  lon = 0.0;
    qreal  lat = 0.0;
    qreal  prevLon = 0.0;
    qreal  prevLat = 0.0;
    Quaternion  qpos;
    Quaternion  prevqpos;

    QVector<GeoDataCoordinates*>::const_iterator  it;
    for ( it = m_pMeasurePointList.constBegin();
          it != m_pMeasurePointList.constEnd();
          ++it )
    {
        qpos = (*it)->quaternion();

        (*it)->geoCoordinates( lon, lat );

        if ( it!= m_pMeasurePointList.constBegin() ) {
            totalDistance += m_model->planetRadius() * distanceSphere( prevLon, prevLat, lon, lat ); 

            drawDistancePath( painter, prevqpos, qpos, viewport );
        }

        prevqpos = qpos;
        prevLon  = lon; 
        prevLat  = lat;
    }

    // Paint the end points of the paths.
    drawDistancePoints( painter, viewport );

    // Paint the total distance in the upper left corner.
    if ( m_pMeasurePointList.size() > 1 )
        drawTotalDistanceLabel( painter, totalDistance );
}

void MeasureTool::drawDistancePoints( GeoPainter *painter, 
                                      ViewportParams *viewport )
{
    int  x = 0;
    int  y = 0;

    // Convenience variables
    int  width  = viewport->width();
    int  height = viewport->height();

    // Paint the marks.
    Quaternion                              qpos;
    QVector<GeoDataCoordinates*>::const_iterator  it;
    for ( it = m_pMeasurePointList.constBegin();
          it != m_pMeasurePointList.constEnd();
          ++it )
    {
        qreal  lon;
        qreal  lat;

        qpos = (*it)->quaternion();
        qpos.getSpherical( lon, lat );

        if ( viewport->currentProjection()
	     ->screenCoordinates( GeoDataCoordinates( lon, lat ), viewport,
				  x, y ) )
	{
            // Don't process markers if they are outside the screen area.
            // FIXME: Some part of it may be visible anyway.
            if ( 0 <= x && x < width 
                 && 0 <= y && y < height )
            {
                drawMark( painter, viewport, x, y );
            }
        }
    }
}

void MeasureTool::drawMark( GeoPainter* painter, ViewportParams *viewport,
                            int x, int y )
{
    const int MarkRadius = 5;

    // Check if the mark is inside the y range of the screen.
    if ( y < 0 || y > viewport->height() )
        return;

    // Some convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();

    // Paint the mark, and repeat it if the projection allows it.
    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->setPen( QColor( Qt::white ) );
    if ( viewport->currentProjection()->repeatX() ) {

	// Calculate the left-most possible X.
	while ( x > 0 ) {
	    x -= 4 * radius;
	}
	x += 4 * radius;

	// Start painting and repeat until we leave the screen.
	while ( x <= width ) {
            // The mark is a simple cross right now.
	    painter->drawLine( x - MarkRadius, y, x + MarkRadius, y );
	    painter->drawLine( x, y - MarkRadius, x, y + MarkRadius );

	    x += 4 * radius;
	}
    }
    else {
	painter->drawLine( x - MarkRadius, y, x + MarkRadius, y );
	painter->drawLine( x, y - MarkRadius, x, y + MarkRadius );
    }
}


void MeasureTool::drawDistancePath( GeoPainter* painter, 
                                    const Quaternion & prevqpos, const Quaternion & qpos,
                                    ViewportParams *viewport )
{
    // Some convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();

    // Create the path.  To get a smooth appearance, split each path
    // into 20 linear segments. This is probably enough to remove all
    // jagginess.
    //
    // FIXME: Implement a real optimized path drawer that splits just
    //        as much as is needed, but not more.
    Quaternion  itpos;
    QPolygonF   distancePath;
    for ( int i = 0; i < 21; ++i ) {
        qreal  t = (qreal)(i) / 20.0;
        qreal  lon = 0.0;
        qreal  lat = 0.0;
        int     x = 0;
        int     y = 0;

        // Let itpos be a quaternion that is between prevqpos and qpos.
        itpos.nlerp( prevqpos, qpos, t );
        itpos.getSpherical( lon, lat );
        viewport->currentProjection()
	     ->screenCoordinates( GeoDataCoordinates( lon, lat ), viewport,
				  x, y );
        distancePath << QPointF( x, y );
    }

    // Now actually paint the path. Repeat it if the projection allows it.
    if ( viewport->currentProjection()->repeatX() ) {
        QPolygonF   path2( distancePath );
        QRectF      rect( path2.boundingRect() );

        int         rightX = (int) rect.right();
        int         leftX  = (int) rect.left();
        int         move   = 0;
        while ( rightX > 0 ) {
            move   += 4 * radius;
            rightX -= 4 * radius ;
            leftX  -= 4 * radius ;
        }
        move   -= 4 * radius;
        rightX += 4 * radius;
        leftX  += 4 * radius;

        path2.translate( (qreal) -move, 0.0 );
        while ( leftX < width ) {
            painter->drawPolyline( path2 );
            path2.translate( (qreal) ( 4 * radius ), 0.0 );

            leftX += 4 * radius;
        }
    } else
        painter->drawPolyline( distancePath );
}

#if 0
// FIXME: Don't remove this yet, since we want to fix
//        multiple drawing of the distance paths for projections that
//        have repeatX() == true over the date line.
//        
void MeasureTool::rectangularDrawDistancePath( GeoPainter *painter,
                                               const Quaternion & prevqpos,
                                               const Quaternion & qpos,
                                               ViewportParams *viewport,
                                               bool antialiasing )
{
    int         imgwidth  = viewport->width();
    int         imgheight = viewport->height();
    int         radius    = viewport->radius();

    qreal      x;
    qreal      y;
    int         currentSign;
    int         previousSign;
    qreal      lon;
    qreal      lat;
    qreal      previousLon;
    qreal      previousY = 0.0;
    qreal      previousX;
    qreal      interpolatedY;

    QPolygonF   distancePath;
    m_visibleArea = QRectF( 0, 0, imgwidth, imgheight );

    Q_UNUSED( antialiasing );

    qreal      t = 0.0;
    Quaternion  itpos;
    //Calculate the sign of the first measurePoint
    itpos.slerp( prevqpos, qpos, t );
    itpos.getSpherical( lon, lat );
    currentSign = previousSign = (lon < 0)?-1:1;
    previousLon = lon;

    Q_UNUSED( antialiasing );

    for ( int i = 0; i < 21; ++i ) {
        t = (qreal)(i) / 20.0;

        itpos.slerp( prevqpos, qpos, t );
        itpos.getSpherical(lon,lat);

        x = (qreal)( imgwidth  / 2 - ( m_centerLon - lon ) * m_rad2Pixel );
        y = (qreal)( imgheight / 2 + ( m_centerLat - lat ) * m_rad2Pixel );

        if ( i == 0 ) {
            previousY = y;
        }

        // The next steps deal with the measurement of two points
        // that the shortest path crosses the dateline
        currentSign = (lon < 0)?-1:1;
        if ( previousSign != currentSign
             && fabs(previousLon) + fabs(lon) > M_PI)
        {
            // FIXME:Fix the interpolation the problem is i think the
            //       (x - previousX) as is too wide
            //
            // It's based on y = y1 + (y2-y1)/(x2-x1)*(x-x1)
            // interpolatedY= previousY + ( y - previousY )
            //                / ( 4*radius - fabs( x - previousX ) )
            //                * ( imgwidth / 2 - m_rad2Pixel * (m_centerLon - lon) + previousSign*2*radius - previousX );
            // This is temporal just to be able to commit
            interpolatedY= ( y + previousY ) / 2;
            distancePath << QPointF( imgwidth / 2 - m_centerLon * m_rad2Pixel
                                    + previousSign * 2 * radius,
                                      interpolatedY );
            drawAndRepeatDistancePath( painter, distancePath );

            distancePath.clear();
            distancePath << QPointF( imgwidth / 2 - m_centerLon * m_rad2Pixel
                                     + currentSign  * 2 * radius,
                                     interpolatedY);
        }
        else 
            distancePath << QPointF( x, y );

        previousSign = currentSign;
        previousLon  = lon;
        previousX = x;
        previousY = y;
    }

    drawAndRepeatDistancePath( painter, distancePath );
}

void MeasureTool::drawAndRepeatDistancePath( GeoPainter* painter,
                                             const QPolygonF distancePath )
{
    //copy the QPolygonF so we can freely translate it
    QPolygonF distancePathMovable(distancePath);

    if ( distancePathMovable.boundingRect().topLeft().x() < 0 
        && distancePathMovable.boundingRect().topRight().x() < 0 ) {
        distancePathMovable.translate( 4 * m_radius, 0 );
    }
    //repeat to right
    while ( m_visibleArea.intersects( distancePathMovable.boundingRect() ) ) {
        painter->drawPolyline( distancePathMovable );
        distancePathMovable.translate( 4 * m_radius, 0 );
    }
    //reset to center position
    distancePathMovable = distancePath;
    //translate to the first left repeating
    distancePathMovable.translate( -4 * m_radius, 0 );
    //repeat to left
    while ( m_visibleArea.intersects( distancePathMovable.boundingRect() ) ) {
        painter->drawPolyline( distancePathMovable );
        distancePathMovable.translate( -4 * m_radius, 0 );
    }
}
#endif

void MeasureTool::drawTotalDistanceLabel( GeoPainter *painter, 
                                          qreal totalDistance )
{
    QString  distanceString;
    if ( totalDistance >= 1000.0 )
        distanceString = tr("Total Distance: %1 km").arg( totalDistance/1000.0 );
    else
        distanceString = tr("Total Distance: %1 m").arg( totalDistance );

    QString distanceUnitString;

    Marble::DistanceUnit distanceUnit;
    distanceUnit = MarbleGlobal::getInstance()->locale()->distanceUnit();

    if ( distanceUnit == Marble::Metric ) {
        if ( totalDistance >= 1000.0 ) {
            distanceString = tr("Total Distance: %1 km").arg( totalDistance/1000.0 );
        }
        else {
            distanceString = tr("Total Distance: %1 m").arg( totalDistance );
        }
    }
    else {
        distanceString = QString("Total Distance: %1 mi").arg( totalDistance/1000.0 * KM2MI );
    }


    painter->setPen( QColor( Qt::black ) );
    painter->setBrush( QColor( 192, 192, 192, 192 ) );

    painter->drawRect( 10, 105, 10 + QFontMetrics( m_font_regular ).boundingRect( distanceString ).width() + 5, 10 + m_fontascent + 2 );
    painter->setFont( m_font_regular );
    painter->drawText( 15, 110 + m_fontascent, distanceString );
}


void MeasureTool::addMeasurePoint( qreal lon, qreal lat )
{
    m_pMeasurePointList << new GeoDataCoordinates( lon, lat );
}


void MeasureTool::removeMeasurePoints()
{
    m_pMeasurePointList.clear();
}

#include "MeasureTool.moc"
