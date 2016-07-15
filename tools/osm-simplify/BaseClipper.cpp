//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "BaseClipper.h"

#include "MarbleMath.h"

#include <QPolygonF>
#include <QDebug>

BaseClipper::BaseClipper() :
    m_left(0.0),
    m_right(0.0),
    m_top(0.0),
    m_bottom(0.0),
    m_currentSector(4),
    m_previousSector(4),
    m_currentPoint(QPointF()),
    m_previousPoint(QPointF())
{

}



QPolygonF BaseClipper::lineString2Qpolygon(const GeoDataLineString& lineString)
{
    QPolygonF polygon;
    foreach (const GeoDataCoordinates& coord, lineString) {
        // Need to flip the Y axis(latitude)
        QPointF point(coord.longitude(), -coord.latitude());
        polygon.append(point);
    }

    return polygon;
}

QPolygonF BaseClipper::linearRing2Qpolygon(const GeoDataLinearRing& linearRing)
{
    QPolygonF polygon;
    foreach (const GeoDataCoordinates& coord, linearRing) {
        // Need to flip the Y axis(latitude)
        QPointF point(coord.longitude(), -coord.latitude());
        polygon.append(point);
    }

    return polygon;
}

GeoDataLineString BaseClipper::qPolygon2lineString(const QPolygonF& polygon)
{
    GeoDataLineString lineString;
    foreach (const QPointF& point, polygon) {
        // Flipping back the Y axis
        GeoDataCoordinates coord(point.x(), -point.y());
        lineString.append(coord);
    }

    return lineString;
}

GeoDataLinearRing BaseClipper::qPolygon2linearRing(const QPolygonF& polygon)
{
    GeoDataLinearRing linearRing;
    foreach (const QPointF& point, polygon) {
        // Flipping back the Y axis
        GeoDataCoordinates coord(point.x(), -point.y());
        linearRing.append(coord);
    }

    return linearRing;
}

qreal BaseClipper::tileX2lon( unsigned int x, unsigned int maxTileX )
{
    return ( (2*M_PI * x) / maxTileX - M_PI );
}

qreal BaseClipper::tileY2lat( unsigned int y, unsigned int maxTileY )
{
    return gd( M_PI - (2*M_PI * y) / maxTileY ) * (90.0 / 85.0511);
}




void BaseClipper::initClipRect (const GeoDataLatLonBox &clippingBox)
{
    m_left   = clippingBox.west();
    m_right  = clippingBox.east();

    // Had to flip the 'Y' axis, because the origo of the coordinate system in which the
    // geographics coordinates are is based in the bottom left corner, while the
    // screencoordinatas on which this clipper operated are in an upper left corner based origo.

    m_top    = -clippingBox.north();
    m_bottom = -clippingBox.south();
}

qreal BaseClipper::_m( const QPointF & start, const QPointF & end )
{
    qreal  divisor = end.x() - start.x();

    // Had to add more zeros, because what is acceptable in screen coordinates
    // could be meters on 10 meters in geographic coordinates.
    if ( std::fabs( divisor ) < 0.00000000000000001 ) {
        divisor = 0.00000000000000001 * (divisor < 0 ? -1 : 1);
    }

    return ( end.y() - start.y() )
            / divisor;
}


QPointF BaseClipper::clipTop( qreal m, const QPointF & point ) const
{
    return QPointF( ( m_top - point.y() ) / m + point.x(), m_top );
}

QPointF BaseClipper::clipLeft( qreal m, const QPointF & point ) const
{
    return QPointF( m_left, ( m_left - point.x() ) * m + point.y() );
}

QPointF BaseClipper::clipBottom( qreal m, const QPointF & point ) const
{
    return QPointF( ( m_bottom - point.y() ) / m + point.x(), m_bottom );
}

QPointF BaseClipper::clipRight( qreal m, const QPointF & point ) const
{
    return QPointF( m_right, ( m_right - point.x() ) * m + point.y() );
}

int BaseClipper::sector( const QPointF & point ) const
{
    // If we think of the image borders as (infinitely long) parallel
    // lines then the plane is divided into 9 sectors.  Each of these
    // sections is identified by a unique keynumber (currentSector):
    //
    //  0 | 1 | 2
    //  --+---+--
    //  3 | 4 | 5 <- sector number "4" represents the onscreen sector / viewport
    //  --+---+--
    //  6 | 7 | 8
    //

    // Figure out the section of the current point.
    int xSector = 1;
    if ( point.x() < m_left )
        xSector = 0;
    else if ( point.x() > m_right )
        xSector = 2;

    int ySector = 3;
    if ( point.y() < m_top )
        ySector = 0;
    else if ( point.y() > m_bottom )
        ySector = 6;

    // By adding xSector and ySector we get a
    // sector number of the values shown in the ASCII-art graph above.
    return ySector + xSector;
}

void BaseClipper::clipPolyObject ( const QPolygonF & polygon,
                                   QVector<QPolygonF> & clippedPolyObjects,
                                   bool isClosed )
{
    // Only create a new polyObject as soon as we know for sure that
    // the current point is on the screen.
    QPolygonF clippedPolyObject = QPolygonF();

    const QVector<QPointF>::const_iterator  itStartPoint = polygon.constBegin();
    const QVector<QPointF>::const_iterator  itEndPoint   = polygon.constEnd();
    QVector<QPointF>::const_iterator        itPoint      = itStartPoint;

    // We use a while loop to be able to cover linestrings as well as linear rings:
    // Linear rings require to tessellate the path from the last node to the first node
    // which isn't really convenient to achieve with a for loop ...

    bool processingLastNode = false;

    while ( itPoint != itEndPoint ) {
        m_currentPoint = (*itPoint);
        // qDebug() << "m_currentPoint.x()" << m_currentPoint.x() << "m_currentPOint.y()" << m_currentPoint.y();

        // Figure out the sector of the current point.
        m_currentSector = sector( m_currentPoint );

        // Initialize the variables related to the previous point.
        if ( itPoint == itStartPoint && processingLastNode == false ) {
            if ( isClosed ) {
                m_previousPoint = polygon.last();

                // Figure out the sector of the previous point.
                m_previousSector = sector( m_previousPoint );
            }
            else {
                m_previousSector = m_currentSector;
            }
        }

        // If the current point reaches a new sector, take care of clipping.
        if ( m_currentSector != m_previousSector ) {
            if ( m_currentSector == 4 || m_previousSector == 4 ) {
                // In this case the current or the previous point is visible on the
                // screen but not both. Hence we only need to clip once and require
                // only one interpolation for both cases.

                clipOnce( clippedPolyObject, clippedPolyObjects, isClosed );
            }
            else {
                // This case mostly deals with lines that reach from one
                // sector that is located off screen to another one that
                // is located off screen. In this situation the line
                // can get clipped once, twice, or not at all.

                clipMultiple( clippedPolyObject, clippedPolyObjects, isClosed );
            }

            m_previousSector = m_currentSector;
        }

        // If the current point is onscreen, just add it to our final polygon.
        if ( m_currentSector == 4 ) {

            clippedPolyObject << m_currentPoint;
        }

        m_previousPoint = m_currentPoint;

        // Now let's handle the case where we have a (closed) polygon and where the
        // last point of the polyline is outside the viewport and the start point
        // is inside the viewport. This needs special treatment
        if ( processingLastNode ) {
            break;
        }
        ++itPoint;

        if ( itPoint == itEndPoint  && isClosed ) {
            itPoint = itStartPoint;
            processingLastNode = true;
        }
    }

    // Only add the pointer if there's node data available.
    if ( !clippedPolyObject.isEmpty() ) {
        clippedPolyObjects << clippedPolyObject;
    }
}


void BaseClipper::clipMultiple( QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                bool isClosed )
{
    Q_UNUSED( clippedPolyObjects )
    Q_UNUSED( isClosed )

    // Take care of adding nodes in the image corners if the iterator
    // traverses off screen sections.

    qreal  m = _m( m_previousPoint, m_currentPoint );

    switch ( m_currentSector ) {
    case 0:
        if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
            if ( pointTop.x() >= m_left && pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top )
                clippedPolyObject << pointLeft;
        }
        else if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
            if ( pointLeft.y() >= m_top && pointLeft.y() < m_bottom )
                clippedPolyObject << pointLeft;
            if ( pointTop.x() > m_left )
                clippedPolyObject << pointTop;
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom )
                clippedPolyObject << pointRight;
            if ( pointTop.x() > m_left && pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom )
                clippedPolyObject << pointLeft;

            if ( pointBottom.x() <= m_left && pointLeft.y() >= m_bottom )
                clippedPolyObject << QPointF( m_left, m_bottom );
            if ( pointTop.x() >= m_right && pointRight.y() <= m_top )
                clippedPolyObject << QPointF( m_right, m_top );
        }

        clippedPolyObject << QPointF( m_left, m_top );
        break;

    case 1:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
            if ( pointTop.x() > m_left )
                clippedPolyObject << pointTop;
        }
        else if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
            if ( pointTop.x() < m_right )
                clippedPolyObject << pointTop;
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointBottom.x() > m_left )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() <= m_bottom )
                clippedPolyObject << pointLeft;
            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
        }
        else if ( m_previousSector == 7 ) {
            clippedPolyObject << clipBottom( m, m_previousPoint );
            clippedPolyObject << clipTop( m, m_currentPoint );
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() <= m_bottom )
                clippedPolyObject << pointRight;
            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
        }
        break;

    case 2:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
            if ( pointTop.x() > m_left && pointTop.x() <= m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top )
                clippedPolyObject << pointRight;
        }
        else if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
            if ( pointRight.y() >= m_top && pointRight.y() < m_bottom )
                clippedPolyObject << pointRight;
            if ( pointTop.x() < m_right )
                clippedPolyObject << pointTop;
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );

            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom )
                clippedPolyObject << pointLeft;
            if ( pointTop.x() > m_left && pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom )
                clippedPolyObject << pointRight;

            if ( pointBottom.x() >= m_right && pointRight.y() >= m_bottom )
                clippedPolyObject << QPointF( m_right, m_bottom );
            if ( pointTop.x() <= m_left && pointLeft.y() <= m_top )
                clippedPolyObject << QPointF( m_left, m_top );
        }

        clippedPolyObject << QPointF( m_right, m_top );
        break;

    case 3:
        if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointTop.x() > m_left )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() < m_bottom )
                clippedPolyObject << pointRight;
            if ( pointBottom.x() > m_left && pointBottom.x() <= m_right )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 5 ) {
            clippedPolyObject << clipRight( m, m_previousPoint );
            clippedPolyObject << clipLeft( m, m_currentPoint );
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() > m_top )
                clippedPolyObject << pointRight;
            if ( pointTop.x() > m_left && pointTop.x() <= m_right )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
        }
        break;

    case 5:
        if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom )
                clippedPolyObject << pointLeft;
            if ( pointBottom.x() >= m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
        }
        else if ( m_previousSector == 3 ) {
            clippedPolyObject << clipLeft( m, m_previousPoint );
            clippedPolyObject << clipRight( m, m_currentPoint );
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() > m_top )
                clippedPolyObject << pointLeft;
            if ( pointTop.x() >= m_left && pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
        }
        break;

    case 6:
        if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
            if ( pointBottom.x() >= m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() < m_bottom )
                clippedPolyObject << pointLeft;
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
            if ( pointLeft.y() > m_top && pointLeft.y() <= m_bottom )
                clippedPolyObject << pointLeft;
            if ( pointBottom.x() > m_left )
                clippedPolyObject << pointBottom;
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointTop.x() > m_left && pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom )
                clippedPolyObject << pointRight;
            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom )
                clippedPolyObject << pointLeft;

            if ( pointBottom.x() >= m_right && pointRight.y() >= m_bottom )
                clippedPolyObject << QPointF( m_right, m_bottom );
            if ( pointTop.x() <= m_left && pointLeft.y() <= m_top )
                clippedPolyObject << QPointF( m_left, m_top );
        }

        clippedPolyObject << QPointF( m_left, m_bottom );
        break;

    case 7:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
            if ( pointBottom.x() > m_left )
                clippedPolyObject << pointBottom;
        }
        else if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
            if ( pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() > m_left )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() >= m_top && pointLeft.y() < m_bottom )
                clippedPolyObject << pointLeft;
            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            clippedPolyObject << clipTop( m, m_previousPoint );
            clippedPolyObject << clipBottom( m, m_currentPoint );
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() >= m_top && pointRight.y() < m_bottom )
                clippedPolyObject << pointRight;
            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
        }
        break;

    case 8:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
            if ( pointBottom.x() > m_left && pointBottom.x() <= m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() < m_bottom )
                clippedPolyObject << pointRight;
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
            if ( pointRight.y() > m_top && pointRight.y() <= m_bottom )
                clippedPolyObject << pointRight;
            if ( pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );

            if ( pointTop.x() > m_left && pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom )
                clippedPolyObject << pointLeft;
            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom )
                clippedPolyObject << pointRight;

            if ( pointBottom.x() <= m_left && pointLeft.y() >= m_bottom )
                clippedPolyObject << QPointF( m_left, m_bottom );
            if ( pointTop.x() >= m_right && pointRight.y() <= m_top )
                clippedPolyObject << QPointF( m_right, m_top );
        }

        clippedPolyObject << QPointF( m_right, m_bottom );
        break;

    default:
        break;
    }
}

void BaseClipper::clipOnceCorner( QPolygonF & clippedPolyObject,
                                  QVector<QPolygonF> & clippedPolyObjects,
                                  const QPointF& corner,
                                  const QPointF& point,
                                  bool isClosed )
{
    if ( m_currentSector == 4) {
        // Appearing
        clippedPolyObject << corner;
        clippedPolyObject << point;
    } else {
        // Disappearing
        clippedPolyObject << point;
        clippedPolyObject << corner;
    }
}

void BaseClipper::clipOnceEdge( QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                const QPointF& point,
                                bool isClosed )
{
    if ( m_currentSector == 4) {
        // Appearing
        if ( !isClosed ) {
            clippedPolyObject = QPolygonF();
        }
        clippedPolyObject << point;
    }
    else {
        // Disappearing
        clippedPolyObject << point;

        if ( !isClosed ) {
            clippedPolyObjects << clippedPolyObject;
            clippedPolyObject = QPolygonF();
        }
    }
}

void BaseClipper::clipOnce( QPolygonF & clippedPolyObject,
                            QVector<QPolygonF> & clippedPolyObjects,
                            bool isClosed )
{
    //	Interpolate border points (linear interpolation)
    QPointF point;

    // Calculating the slope.
    qreal m = _m( m_previousPoint, m_currentPoint );

    // Calculate in which sector the end of the line is located that is off screen
    int offscreenpos = ( m_currentSector == 4 ) ? m_previousSector : m_currentSector;

    // "Rise over run" for all possible situations .
    switch ( offscreenpos ) {
    case 0: // topleft
        point = clipTop( m, m_previousPoint );
        if ( point.x() < m_left ) {
            point = clipLeft( m, point );
        }
        clipOnceCorner( clippedPolyObject, clippedPolyObjects, QPointF( m_left, m_top ), point, isClosed );
        break;
    case 1: // top
        point = clipTop( m, m_previousPoint );
        clipOnceEdge( clippedPolyObject, clippedPolyObjects, point, isClosed );
        break;
    case 2: // topright
        point = clipTop( m, m_previousPoint );
        if ( point.x() > m_right ) {
            point = clipRight( m, point );
        }
        clipOnceCorner( clippedPolyObject, clippedPolyObjects, QPointF( m_right, m_top ), point, isClosed );
        break;
    case 3: // left
        point = clipLeft( m, m_previousPoint );
        clipOnceEdge( clippedPolyObject, clippedPolyObjects, point, isClosed );
        break;
    case 5: // right
        point = clipRight( m, m_previousPoint );
        clipOnceEdge( clippedPolyObject, clippedPolyObjects, point, isClosed );
        break;
    case 6: // bottomleft
        point = clipBottom( m, m_previousPoint );
        if ( point.x() < m_left ) {
            point = clipLeft( m, point );
        }
        clipOnceCorner( clippedPolyObject, clippedPolyObjects, QPointF( m_left, m_bottom ), point, isClosed );
        break;
    case 7: // bottom
        point = clipBottom( m, m_previousPoint );
        clipOnceEdge( clippedPolyObject, clippedPolyObjects, point, isClosed );
        break;
    case 8: // bottomright
        point = clipBottom( m, m_previousPoint );
        if ( point.x() > m_right ) {
            point = clipRight( m, point );
        }
        clipOnceCorner( clippedPolyObject, clippedPolyObjects, QPointF( m_right, m_bottom ), point, isClosed );
        break;
    default:
        break;
    }

}
