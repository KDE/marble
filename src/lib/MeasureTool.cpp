//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2007-2008 Carlos Licea <carlos.licea@kdemail.net>
//


// local
#include "MeasureTool.h"

// posix
#include <cmath>

// Qt
#include <QtCore/QPoint>
#include <QtGui/QPainter>

// marble
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "MarbleLocale.h"
#include "AbstractProjection.h"
#include "GeoPainter.h"
#include "Planet.h"
#include "ViewportParams.h"

#include "global.h"

using namespace Marble;

MeasureTool::MeasureTool( MarbleModel *model, QObject* parent )
    : QObject( parent ),
      m_measureLineString( GeoDataLineString( Tessellate ) ),
      m_model( model ),
#ifdef Q_OS_MACX
      m_font_regular( QFont( "Sans Serif", 10, 50, false ) ),
#else
      m_font_regular( QFont( "Sans Serif",  8, 50, false ) ),
#endif
      m_fontascent( QFontMetrics( m_font_regular ).ascent() ),
      m_pen( Qt::red )
{
    m_pen.setWidthF( 2.0 );
}


bool MeasureTool::render( GeoPainter *painter, 
                          ViewportParams *viewport,
                          const QString& renderPos,
                          GeoSceneLayer * layer )
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    // FIXME: Add this stuff into the Layermanager as something to be 
    // called before the float items.

    bool antialiasing = false;

    if (   painter->mapQuality() == HighQuality
        || painter->mapQuality() == PrintQuality ) {
            antialiasing = true;
    }

    // No way to paint anything if the list is empty.
    if ( m_measureLineString.isEmpty() )
        return true;

    // Prepare for painting the measure line string and paint it.
    painter->setRenderHint( QPainter::Antialiasing, antialiasing );
    painter->setPen( m_pen );

    painter->drawPolyline( m_measureLineString );

    // Paint the nodes of the paths.
    drawMeasurePoints( painter, viewport );

    // Paint the total distance in the upper left corner.
    qreal  totalDistance = m_measureLineString.length( m_model->planet()->radius() );

    if ( m_measureLineString.size() > 1 )
        drawTotalDistanceLabel( painter, totalDistance );

    return true;
}

void MeasureTool::drawMeasurePoints( GeoPainter *painter,
                                      ViewportParams *viewport )
{
    qreal  y = 0.0;

    // Paint the marks.
    GeoDataLineString::const_iterator it = m_measureLineString.constBegin();
    GeoDataLineString::const_iterator const end = m_measureLineString.constEnd();
    for (; it != end; ++it )
    {
        qreal  lon;
        qreal  lat;

        it->geoCoordinates( lon, lat );

        // FIXME: Replace all of this by some appropriate drawPlaceMark( GeoDataCrossHairs )
        //        or drawPlaceMark( GeoDataPlacemark ) method
        int pointRepeatNum = 0;
        bool globeHidesPoint = false;
        qreal * x  = new qreal[100];

        bool visible = viewport->currentProjection()->screenCoordinates( GeoDataCoordinates( lon, lat ), viewport, x, y, pointRepeatNum, globeHidesPoint );

        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                drawMark( painter, x[it], y );
            }
        }

        delete[] x;
    }
}

void MeasureTool::drawMark( GeoPainter* painter, int x, int y )
{
    const int MarkRadius = 5;

    // Paint the mark, and repeat it if the projection allows it.
    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->setPen( QColor( Qt::white ) );

    painter->drawLine( x - MarkRadius, y, x + MarkRadius, y );
    painter->drawLine( x, y - MarkRadius, x, y + MarkRadius );
}

void MeasureTool::drawTotalDistanceLabel( GeoPainter *painter, 
                                          qreal totalDistance )
{
    QString  distanceString;

    DistanceUnit distanceUnit;
    distanceUnit = MarbleGlobal::getInstance()->locale()->distanceUnit();

    if ( distanceUnit == Meter ) {
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
    m_measureLineString << GeoDataCoordinates( lon, lat );

    emit numberOfMeasurePointsChanged( m_measureLineString.size() );
}

void MeasureTool::removeLastMeasurePoint()
{
    if (!m_measureLineString.isEmpty())
	m_measureLineString.remove( m_measureLineString.size() - 1 );

    emit numberOfMeasurePointsChanged( m_measureLineString.size() );
}

void MeasureTool::removeMeasurePoints()
{
    m_measureLineString.clear();

    emit numberOfMeasurePointsChanged( m_measureLineString.size() );
}

#include "MeasureTool.moc"
