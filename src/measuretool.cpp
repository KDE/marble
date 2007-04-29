#include "measuretool.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtCore/QDebug>
#include <QtGui/QPainter>

#include "Quaternion.h"
#include "clippainter.h"


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
    m_pen.setWidthF( 2.0f );
}


void MeasureTool::paintMeasurePoints(ClipPainter* painter, 
                                     int imgrx, int imgry, int radius, 
                                     Quaternion rotAxis, bool antialiasing )
{
    if ( m_pMeasurePointList.isEmpty() )
        return;

    int  imgwidth  = 2 * imgrx;
    int  imgheight = 2 * imgry;
    int  x = 0;
    int  y = 0;
    int  lastx = 0;
    int  lasty = 0; 

    Quaternion  invRotAxis = rotAxis.inverse();
    Quaternion  qpos;
    Quaternion  prevqpos;

    if ( antialiasing == true )
        painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setPen( m_pen );	
    // painter->setPen(QColor(Qt::white));	
    // painter->drawPolyline(distancePathShadow);

    QVector<GeoPoint*>::const_iterator  it;

    QPolygonF  measurePoints;
    // QPolygonF distancePathShadow;

    m_totalDistance = 0.0f;

    float  lng;
    float  lat;
    float  prevlng;
    float  prevlat;
    prevlng = 0.0f;
    prevlat = 0.0f;

    QVector<QPolygonF>  distancePaths;

    for ( it = m_pMeasurePointList.constBegin();
          it != m_pMeasurePointList.constEnd();
          it++ )
    {
        qpos = (*it)->quaternion();
        qpos.rotateAroundAxis( invRotAxis );

        if ( qpos.v[Q_Z] > 0 ) {
            x = (int)( imgrx + radius*qpos.v[Q_X] );
            y = (int)( imgry + radius*qpos.v[Q_Y] );	

            measurePoints << QPointF( x, y );
            // distancePathShadow << QPointF(x,y+1);
        }

        (*it)->geoCoordinates( lng, lat );

        if ( it!= m_pMeasurePointList.constBegin() ) {
            m_totalDistance += acos( sin( prevlat ) * sin( lat )
                                     + cos( prevlat ) * cos( lat ) * cos( prevlng - lng ) ) * 6371221.0f;

            drawDistancePath( painter, prevqpos, qpos, imgrx, imgry, radius, antialiasing );
        }

        prevqpos = qpos;
        prevlng  = lng; 
        prevlat  = lat;
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
        qpos.rotateAroundAxis( invRotAxis );

        if ( qpos.v[Q_Z] > 0 ) {

            x = (int)( imgrx + radius * qpos.v[Q_X] );
            y = (int)( imgry + radius * qpos.v[Q_Y] );

            // Don't process placemarks if they are outside the screen area.
            if ( x >= 0 && x < imgwidth && y >= 0 && y < imgheight ) {
                paintMark( painter, x, y );	
            }
        }
    }

    if ( m_pMeasurePointList.size() > 1 )
        paintTotalDistanceLabel( painter, imgrx, imgry, m_totalDistance );
}


void MeasureTool::paintTotalDistanceLabel( ClipPainter * painter, 
                                           int imgrx, int imgry, 
                                           float totalDistance )
{
    // if ( totalDistance == m_totalDistance)
    QString  distanceValueString;

    if ( totalDistance >= 1000.0 )
        distanceValueString = QString("%1 km").arg(totalDistance/1000.0f);
    else	
        distanceValueString = QString("%1 m").arg(totalDistance);

    QString  distanceString = QString( "Total Distance: %1" ).arg( distanceValueString );

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


void MeasureTool::drawDistancePath( ClipPainter* painter, Quaternion prevqpos,
                                    Quaternion qpos, int imgrx, int imgry, 
                                    int radius, bool antialiasing )
{
    float       t = 0.0f;
    Quaternion  itpos;

    float       x;
    float       y;
    QPolygonF   distancePath;

    for ( int i = 0; i < 21; ++i ) {
        t = (float)(i) / 20.0f;

        itpos.slerp( prevqpos, qpos, t );

        if ( itpos.v[Q_Z] > 0 ) {
            x = (float)(imgrx) + (float)(radius) * itpos.v[Q_X];
            y = (float)(imgry) + (float)(radius) * itpos.v[Q_Y];

            // paintMark( painter, x, y );
            distancePath << QPointF( x, y );
        }
    }

    painter->drawPolyline( distancePath );
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


#ifndef Q_OS_MACX
#include "measuretool.moc"
#endif
