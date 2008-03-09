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
#include "Quaternion.h"
#include "ViewportParams.h"

#include "global.h"

MeasureTool::MeasureTool( QObject* parent )
    : QObject( parent )
{
    m_totalDistance = 0;

#ifdef Q_OS_MACX
    m_font_regular = QFont( "Sans Serif", 10, 50, false );
#else
    m_font_regular = QFont( "Sans Serif",  8, 50, false );
#endif

    m_fontheight = QFontMetrics( m_font_regular ).height();
    m_fontascent = QFontMetrics( m_font_regular ).ascent();

    m_linecolor  = QColor( 255, 255, 255, 255 );

    m_useworkaround = testbug();

    m_pen.setColor( QColor( Qt::red ) );
    m_pen.setWidthF( 2.0 );
}

void MeasureTool::paintMeasurePoints( ClipPainter *painter, 
                                      ViewportParams *viewport,
                                      bool antialiasing )
{
    if ( m_pMeasurePointList.isEmpty() )
        return;

    // Prepare the painter for the paths.
    painter->setRenderHint( QPainter::Antialiasing, antialiasing );
    painter->setPen( m_pen );

    // Paint the paths and calculate the total length
    m_totalDistance = 0.0;
    double  lon = 0.0;
    double  lat = 0.0;
    double  prevLon = 0.0;
    double  prevLat = 0.0;
    Quaternion  qpos;
    Quaternion  prevqpos;

    QVector<GeoDataPoint*>::const_iterator  it;
    for ( it = m_pMeasurePointList.constBegin();
          it != m_pMeasurePointList.constEnd();
          ++it )
    {
        qpos = (*it)->quaternion();
        //qpos.rotateAroundAxis( invplanetAxis );

        (*it)->geoCoordinates( lon, lat );

        if ( it!= m_pMeasurePointList.constBegin() ) {
            m_totalDistance += acos( sin( prevLat ) * sin( lat )
                                     + cos( prevLat ) * cos( lat ) * cos( prevLon - lon ) ) * EARTH_RADIUS;

            drawDistancePath( painter, prevqpos, qpos, viewport, antialiasing );
        }

        prevqpos = qpos;
        prevLon  = lon; 
        prevLat  = lat;
    }

    // Paint the Measure Points
    switch( viewport->projection() ) {
        case Spherical:
            sphericalPaintMeasurePoints( painter, viewport, antialiasing );
            break;
        case Equirectangular:
            rectangularPaintMeasurePoints( painter, viewport, antialiasing );
            break;
    }

    // Paint the total distance in the upper left corner.
    if ( m_pMeasurePointList.size() > 1 )
        paintTotalDistanceLabel( painter, m_totalDistance );
}

void MeasureTool::sphericalPaintMeasurePoints( ClipPainter *painter, 
                                               ViewportParams *viewport,
                                               bool antialiasing )
{
    int  x = 0;
    int  y = 0;

    Quaternion  invplanetAxis = viewport->planetAxis().inverse();
    Quaternion  qpos;

    if ( antialiasing == true )
        painter->setRenderHint( QPainter::Antialiasing, false );

    // Paint the marks.
    QVector<GeoDataPoint*>::const_iterator  it;
    for ( it = m_pMeasurePointList.constBegin();
          it != m_pMeasurePointList.constEnd();
          ++it )
    {
        qpos = (*it)->quaternion();
        qpos.rotateAroundAxis( invplanetAxis );

        if ( qpos.v[Q_Z] > 0 ) {

            x = (int)( viewport->width()  / 2 
                       + viewport->radius() * qpos.v[Q_X] );
            y = (int)( viewport->height() / 2
                       - viewport->radius() * qpos.v[Q_Y] );

            // Don't process markers if they are outside the screen area.
            // FIXME: Some part of it may be visible anyway.
            if ( x >= 0 && x < viewport->width() 
                 && y >= 0 && y < viewport->height() )
            {
                paintMark( painter, x, y );
            }
        }
    }
}

void MeasureTool::rectangularPaintMeasurePoints( ClipPainter *painter,
                                                 ViewportParams *viewport,
                                                 bool antialiasing )
{
    int  x = 0;
    int  y = 0;

    // Calculate translation of center point
    viewport->centerCoordinates( m_centerLon, m_centerLat );

    m_radius    = viewport->radius();
    m_rad2Pixel = 2 * m_radius / M_PI;


    // FIXME: wtf?
    if ( antialiasing == true )
        painter->setRenderHint( QPainter::Antialiasing, false );

    Quaternion  qpos;
    QVector<GeoDataPoint*>::const_iterator  it;
    for ( it = m_pMeasurePointList.constBegin(); 
          it != m_pMeasurePointList.constEnd();
          ++it )
    {
        double  lon = 0.0;
        double  lat = 0.0;

        qpos = (*it)->quaternion();
        qpos.getSpherical( lon, lat );

        x = (int)( viewport->width()  / 2
                   - ( m_centerLon - lon ) * m_rad2Pixel );
        y = (int)( viewport->height() / 2
                   + ( m_centerLat - lat ) * m_rad2Pixel );

        rectangularPaintMark( painter, x, y, 
                              viewport->width(), viewport->height() );
    }
}

void MeasureTool::paintTotalDistanceLabel( ClipPainter *painter, 
                                           double totalDistance )
{
    QString  distanceString;
    if ( totalDistance >= 1000.0 )
        distanceString = tr("Total Distance: %1 km").arg( totalDistance/1000.0 );
    else
        distanceString = tr("Total Distance: %1 m").arg( totalDistance );

    painter->setPen( QColor( Qt::black ) );
    painter->setBrush( QColor( 192, 192, 192, 192 ) );

    painter->drawRect( 10, 5, 10 + QFontMetrics( m_font_regular ).boundingRect( distanceString ).width() + 5, 10 + m_fontascent + 2 );
    painter->setFont( m_font_regular );
    painter->drawText( 15, 10 + m_fontascent, distanceString );
}


void MeasureTool::paintMark( ClipPainter* painter, int x, int y )
{
    int  halfsize = 5;

    painter->setPen( QColor( Qt::white ) );
    painter->drawLine( x - halfsize, y, x + halfsize, y );
    painter->drawLine( x, y - halfsize, x, y + halfsize );
}

void MeasureTool::rectangularPaintMark( ClipPainter* painter, int x, int y,
                                        int width, int height )
{
    //calls PaintMark multiple times so we can repeat the x mark
    //for the rectangular projection

    //check if it's inside the y range of the screen
    if ( y > height || y < 0 )
        return;
    //calculate the left-most possible X
    while ( x > 0 ) {
        x-=4*m_radius;
    }
    x+=4*m_radius;
    //start painting and repeating until we leave the screen
    while ( x <= width ) {
        paintMark( painter, x, y );
        x+=4*m_radius;
    }
}

void MeasureTool::drawDistancePath( ClipPainter* painter, 
                                    Quaternion prevqpos, Quaternion qpos,
                                    ViewportParams *viewport,
                                    bool antialiasing )
{
    Q_UNUSED( antialiasing );

    Quaternion  itpos;
    QPolygonF   distancePath;
    for ( int i = 0; i < 21; ++i ) {
        double  t = (double)(i) / 20.0;
        double  lon = 0.0;
        double  lat = 0.0;
        int     x = 0;
        int     y = 0;

        // Let itpos be a quaternion that is between prevqpos and qpos.
        itpos.slerp( prevqpos, qpos, t );
        itpos.getSpherical( lon, lat );
        if ( viewport->currentProjection()
	     ->screenCoordinates( lon, lat, viewport,
				  x, y,
				  originalCoordinates /*mappedCoordinates*/ ) )
        {
            //paintMark( painter, x, y );
            distancePath << QPointF( x, y );
        }
        else {
            //qDebug() << "(x,y): " << x << y;
        }
    }

    painter->drawPolyline( distancePath );
}

#if 0
// FIXME: Don't remove this yet, since we probably want to add
//        multiple drawing of the distance paths for projections that
//        have repeatX() == true.
void MeasureTool::rectangularDrawDistancePath( ClipPainter *painter,
                                               Quaternion   prevqpos,
                                               Quaternion   qpos,
                                               ViewportParams *viewport,
                                               bool antialiasing )
{
    int         imgwidth  = viewport->width();
    int         imgheight = viewport->height();
    int         radius    = viewport->radius();

    double      x;
    double      y;
    int         currentSign;
    int         previousSign;
    double      lon;
    double      lat;
    double      previousLon;
    double      previousY = 0.0;
    double      previousX;
    double      interpolatedY;

    QPolygonF   distancePath;
    m_visibleArea = QRectF( 0, 0, imgwidth, imgheight );

    Q_UNUSED( antialiasing );

    double      t = 0.0;
    Quaternion  itpos;
    //Calculate the sign of the first measurePoint
    itpos.slerp( prevqpos, qpos, t );
    itpos.getSpherical( lon, lat );
    currentSign = previousSign = (lon < 0)?-1:1;
    previousLon = lon;

    Q_UNUSED( antialiasing );

    for ( int i = 0; i < 21; ++i ) {
        t = (double)(i) / 20.0;

        itpos.slerp( prevqpos, qpos, t );
        itpos.getSpherical(lon,lat);

        x = (double)( imgwidth  / 2 - ( m_centerLon - lon ) * m_rad2Pixel );
        y = (double)( imgheight / 2 + ( m_centerLat - lat ) * m_rad2Pixel );

        if ( i == 0 ) {
            previousY = y;
        }

        //The next steeps deal with the measurement of two points
        //that the shortest path crosses the dateline
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
#endif
void MeasureTool::drawAndRepeatDistancePath( ClipPainter* painter,
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

bool MeasureTool::testbug()
{
    QString  testchar( "K" );
    QFont    font( "Sans Serif", 10 );

    int fontheight = QFontMetrics( font ).height();
    int fontwidth  = QFontMetrics( font ).width( testchar );
    int fontascent = QFontMetrics( font ).ascent();

    QPixmap  pixmap ( fontwidth, fontheight );
    pixmap.fill( Qt::transparent );

    QPainter  textpainter;
    textpainter.begin( &pixmap );
    textpainter.setPen( QColor( 0, 0, 0, 255 ) );
    textpainter.setFont( font );
    textpainter.drawText( 0, fontascent, testchar );
    textpainter.end();

    QImage  image = pixmap.toImage();

    for ( int x = 0; x < fontwidth; ++x )
        for ( int y = 0; y < fontheight; ++y ) {
            if ( qAlpha( image.pixel( x, y ) ) > 0 )
                return false;
        }

    return true;
}


#include "MeasureTool.moc"
