//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "MeasureTool.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtGui/QPainter>

#include "Quaternion.h"


MeasureTool::MeasureTool(QObject* parent)
    : QObject(parent)
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

void MeasureTool::paintMeasurePoints( ClipPainter* painter, 
                                     int imgrx, int imgry, int radius, 
                                     Quaternion planetAxis, bool antialiasing, 
                                     Projection currentProjection )
{
    switch( currentProjection ) {
        case Spherical:
            sphericalPaintMeasurePoints( painter, imgrx, imgry, radius, planetAxis, antialiasing );
            break;
        case Equirectangular:
            rectangularPaintMeasurePoints( painter, imgrx, imgry, radius, planetAxis, antialiasing );
            break;
    }
}

void MeasureTool::sphericalPaintMeasurePoints(ClipPainter* painter, 
                                     int imgrx, int imgry, int radius, 
                                     Quaternion planetAxis, bool antialiasing )
{
    if ( m_pMeasurePointList.isEmpty() )
        return;

    int  imgwidth  = 2 * imgrx;
    int  imgheight = 2 * imgry;
    int  x = 0;
    int  y = 0;
    //int  lastx = 0;
    //int  lasty = 0; 

    Quaternion  invplanetAxis = planetAxis.inverse();
    Quaternion  qpos;
    Quaternion  prevqpos;

    if ( antialiasing == true )
        painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setPen( m_pen );
    // painter->setPen(QColor(Qt::white));
    // painter->drawPolyline(distancePathShadow);

    QVector<GeoDataPoint*>::const_iterator  it;

    // QPolygonF  measurePoints;
    // QPolygonF distancePathShadow;

    m_totalDistance = 0.0;

    double  lon;
    double  lat;
    double  prevLon = 0.0;
    double  prevLat = 0.0;

    QVector<QPolygonF>  distancePaths;

    for ( it = m_pMeasurePointList.constBegin();
          it != m_pMeasurePointList.constEnd();
          it++ )
    {
        qpos = (*it)->quaternion();
        qpos.rotateAroundAxis( invplanetAxis );
// FIXME: This is not needed, shouldn't it be removed?
//         if ( qpos.v[Q_Z] > 0 ) {
//             x = (int)( imgrx + radius*qpos.v[Q_X] );
//             y = (int)( imgry + radius*qpos.v[Q_Y] );
// 
//             measurePoints << QPointF( x, y );
//             // distancePathShadow << QPointF(x,y+1);
//         }

        (*it)->geoCoordinates( lon, lat );

        if ( it!= m_pMeasurePointList.constBegin() ) {
            m_totalDistance += acos( sin( prevLat ) * sin( lat )
                                     + cos( prevLat ) * cos( lat ) * cos( prevLon - lon ) ) * 6371221.0;

            drawDistancePath( painter, prevqpos, qpos, imgrx, imgry, radius, antialiasing, Spherical );
        }

        prevqpos = qpos;
        prevLon  = lon; 
        prevLat  = lat;
    }

    if ( antialiasing == true )
        painter->setRenderHint( QPainter::Antialiasing, false );
#if 0
      if (antialiasing == true) painter->setRenderHint(QPainter::Antialiasing, true);
      painter->setPen( QColor( Qt::red ) );
      painter->drawPolyline( measurePoints );
      // painter->setPen( QColor( Qt::white ) );
      // painter->drawPolyline( distancePathShadow );
      if ( antialiasing == true )
          painter->setRenderHint( QPainter::Antialiasing, false );
#endif

    for ( it = m_pMeasurePointList.constBegin();
          it != m_pMeasurePointList.constEnd();
          it++ )
    {
        qpos = (*it)->quaternion();
        qpos.rotateAroundAxis( invplanetAxis );

        if ( qpos.v[Q_Z] > 0 ) {

            x = (int)( imgrx + radius * qpos.v[Q_X] );
            y = (int)( imgry + radius * qpos.v[Q_Y] );

//             Don't process placemarks if they are outside the screen area.
            if ( x >= 0 && x < imgwidth && y >= 0 && y < imgheight ) {
                paintMark( painter, x, y );
            }
        }
    }

    if ( m_pMeasurePointList.size() > 1 )
        paintTotalDistanceLabel( painter, imgrx, imgry, m_totalDistance );
}

void MeasureTool::rectangularPaintMeasurePoints(ClipPainter* painter, 
                                     int imgrx, int imgry, int radius, 
                                     Quaternion planetAxis, bool antialiasing )
{
    if ( m_pMeasurePointList.isEmpty() )
        return;

    int  imgwidth  = 2 * imgrx;
    int  imgheight = 2 * imgry;
    int  x = 0;
    int  y = 0;

    // Calculate translation of center point
    m_centerLat =  planetAxis.pitch() + M_PI;
    if ( m_centerLat > M_PI ) m_centerLat -= 2 * M_PI; 
    m_centerLon =  planetAxis.yaw() + M_PI;

    m_xyFactor = 2*radius / M_PI;
    m_radius = radius;

    Quaternion  qpos;
    Quaternion  prevqpos;

    if ( antialiasing == true )
        painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setPen( m_pen );
    // painter->setPen(QColor(Qt::white));
    // painter->drawPolyline(distancePathShadow);

    QVector<GeoDataPoint*>::const_iterator  it;

    // QPolygonF  measurePoints;
    // QPolygonF distancePathShadow;

    m_totalDistance = 0.0;

    double  prevLon = 0.0;
    double  prevLat = 0.0;
    double  lon = 0.0;
    double  lat = 0.0;

    QVector<QPolygonF>  distancePaths;

    for ( it = m_pMeasurePointList.constBegin();
          it != m_pMeasurePointList.constEnd();
          it++ )
    {
        qpos = (*it)->quaternion();

        (*it)->geoCoordinates( lon, lat );

        if ( it!= m_pMeasurePointList.constBegin() ) {
            m_totalDistance += acos( sin( prevLat ) * sin( lat )
                                     + cos( prevLat ) * cos( lat ) * cos( prevLon - lon ) ) * 6371221.0;

            drawDistancePath( painter, prevqpos, qpos, imgrx, imgry, radius, antialiasing, Equirectangular );
        }

        prevqpos = qpos;
        prevLon  = lon; 
        prevLat  = lat;
    }

    if ( antialiasing == true )
        painter->setRenderHint( QPainter::Antialiasing, false );
#if 0
      if (antialiasing == true) painter->setRenderHint(QPainter::Antialiasing, true);
      painter->setPen( QColor( Qt::red ) );
      painter->drawPolyline( measurePoints );
      // painter->setPen( QColor( Qt::white ) );
      // painter->drawPolyline( distancePathShadow );
      if ( antialiasing == true )
          painter->setRenderHint( QPainter::Antialiasing, false );
#endif

    for ( it = m_pMeasurePointList.constBegin(); 
          it != m_pMeasurePointList.constEnd();
          it++ )
    {
        qpos = (*it)->quaternion();
        qpos.getSpherical(lon,lat);

        x = (int)( imgrx + (lon + m_centerLon ) *m_xyFactor );
        y = (int)( imgry + (lat + m_centerLat ) *m_xyFactor );

        rectangularPaintMark( painter, x, y, imgwidth, imgheight );
    }

    if ( m_pMeasurePointList.size() > 1 )
        paintTotalDistanceLabel( painter, imgrx, imgry, m_totalDistance );
}

void MeasureTool::paintTotalDistanceLabel( ClipPainter * painter, 
                                           int imgrx, int imgry, 
                                           double totalDistance )
{
    Q_UNUSED( imgrx );
    Q_UNUSED( imgry );

    // if ( totalDistance == m_totalDistance)
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
    painter->drawLine( x - halfsize, y, x + halfsize , y );
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

void MeasureTool::drawDistancePath( ClipPainter* painter, Quaternion prevqpos,
                                    Quaternion qpos, int imgrx, int imgry, 
                                    int radius, bool antialiasing, Projection currentProjection )
{
    switch( currentProjection ) {
        case Spherical:
            sphericalDrawDistancePath( painter, prevqpos, qpos, imgrx, imgry, radius, antialiasing );
            break;
        case Equirectangular:
            rectangularDrawDistancePath( painter, prevqpos, qpos, imgrx, imgry, radius, antialiasing );
            break;
    }
}

void MeasureTool::sphericalDrawDistancePath( ClipPainter* painter, Quaternion prevqpos,
                                    Quaternion qpos, int imgrx, int imgry, 
                                    int radius, bool antialiasing )
{
    double      t = 0.0;
    Quaternion  itpos;

    double      x;
    double      y;
    QPolygonF   distancePath;

    Q_UNUSED( antialiasing );

    for ( int i = 0; i < 21; ++i ) {
        t = (double)(i) / 20.0;

        itpos.slerp( prevqpos, qpos, t );

        if ( itpos.v[Q_Z] > 0 ) {
            x = (double)(imgrx) + (double)(radius) * itpos.v[Q_X];
            y = (double)(imgry) + (double)(radius) * itpos.v[Q_Y];

            // paintMark( painter, x, y );
            distancePath << QPointF( x, y );
        }
    }

    painter->drawPolyline( distancePath );
}

void MeasureTool::rectangularDrawDistancePath( ClipPainter* painter, Quaternion prevqpos,
                                    Quaternion qpos, int imgrx, int imgry, 
                                    int radius, bool antialiasing )
{
    double      x;
    double      y;
    int         currentSign;
    int         previousSign;
    double      lon;
    double      lat;
    double      previousLon;
    double      previousY;
    double      previousX;
    double      interpolatedY;

    double      centerLonPixel = m_centerLon * m_xyFactor;
    double      centerLatPixel = m_centerLat * m_xyFactor;
    QPolygonF   distancePath;
    m_visibleArea = QRectF( 0, 0, imgrx*2, imgry*2 );

    Q_UNUSED( antialiasing );

    double      t = 0.0;
    Quaternion  itpos;
    //Calculate the sign of the first measurePoint
    itpos.slerp( prevqpos, qpos, t );
    itpos.getSpherical(lon,lat);
    currentSign = previousSign = (lon<0)?-1:1;
    previousLon = lon;

    Q_UNUSED( antialiasing );

    for ( int i = 0; i < 21; ++i ) {
        t = (double)(i) / 20.0;

        itpos.slerp( prevqpos, qpos, t );
        itpos.getSpherical(lon,lat);

        x = (double)( imgrx + ( lon ) *m_xyFactor + centerLonPixel );
        y = (double)( imgry + ( lat ) *m_xyFactor + centerLatPixel );
        //The next steeps deal with the measurement of two points
        //that the shortest path crosses the dateline
        currentSign = (lon < 0)?-1:1;
        if( previousSign != currentSign && fabs(previousLon) + fabs(lon) > M_PI) {
            //FIXME:Fix the interpolation the problem is i think the (x - previousX) as is too wide
//          It's based on y = y1 + (y2-y1)/(x2-x1)*(x-x1)
//             interpolatedY= previousY + ( y - previousY ) /
//                             ( x - previousX ) *
//                                 ( imgrx + previousSign*2*radius - previousX );
            //This is temporal just to be able to commit
            interpolatedY= ( y + previousY )/2;
            distancePath << QPointF( imgrx + centerLonPixel
                                    + previousSign*2*radius,
                                      interpolatedY );
            drawAndRepeatDistancePath( painter, distancePath );
            distancePath.clear();
            distancePath << QPointF( imgrx + centerLonPixel
                                    + currentSign*2*radius,
                                      interpolatedY);
        }
        else distancePath << QPointF( x, y );
        previousSign = currentSign;
        previousLon = lon;
        previousX = x;
        previousY = y;
    }

    drawAndRepeatDistancePath( painter, distancePath );
}

void MeasureTool::drawAndRepeatDistancePath( ClipPainter* painter,
                                             const QPolygonF distancePath )
{
    //copy the QPolygonF so we can freely translate it
    QPolygonF distancePathMovable(distancePath);
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
