//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Adrian Draghici <draghici.adrian.b@gmail.com>
//

// Self
#include "GroundOverlayFrame.h"

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataTypes.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "SceneGraphicsTypes.h"
#include "TextureLayer.h"
#include "MarbleDirs.h"

// Qt
#include <qmath.h>


namespace Marble
{

GroundOverlayFrame::GroundOverlayFrame( GeoDataPlacemark *placemark,
                                        GeoDataGroundOverlay *overlay,
                                        TextureLayer *textureLayer ) :
    SceneGraphicsItem( placemark ),
    m_overlay( overlay ),
    m_textureLayer( textureLayer ),
    m_movedHandle( NoRegion ),
    m_hoveredHandle( NoRegion ),
    m_editStatus( Resize ),
    m_editStatusChangeNeeded( false ),
    m_previousRotation( 0.0 ),
    m_viewport( 0 )
{
    m_resizeIcons.reserve(16);
    // NorthWest
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-diagonal-topleft.png")));
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-diagonal-topleft-active.png")));
    // SouthWest
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-diagonal-topright.png")));
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-diagonal-topright-active.png")));
    // SouthEast
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-diagonal-topleft.png")));
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-diagonal-topleft-active.png")));
    // NorthEast
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-diagonal-topright.png")));
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-diagonal-topright-active.png")));
    // North
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-vertical.png")));
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-vertical-active.png")));
    // South
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-vertical.png")));
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-vertical-active.png")));
    // East
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-horizontal.png")));
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-horizontal-active.png")));
    // West
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-horizontal.png")));
    m_resizeIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-horizontal-active.png")));


    m_rotateIcons.reserve(16);
    // NorthWest
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-rotation-topleft.png")));
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-rotation-topleft-active.png")));
    // SouthWest
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-rotation-bottomleft.png")));
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-rotation-bottomleft-active.png")));
    // SouthEast
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-rotation-bottomright.png")));
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-rotation-bottomright-active.png")));
    // NorthEast
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-rotation-topright.png")));
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-rotation-topright-active.png")));
    // North
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-horizontal.png")));
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-horizontal-active.png")));
    // South
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-horizontal.png")));
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-horizontal-active.png")));
    // East
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-vertical.png")));
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-vertical-active.png")));
    // West
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-vertical.png")));
    m_rotateIcons.append(QImage(MarbleDirs::systemPath() + QLatin1String("/bitmaps/editarrows/arrow-vertical-active.png")));

    update();
    setPaintLayers(QStringList() << "GroundOverlayFrame");
}

void GroundOverlayFrame::paint(GeoPainter *painter, const ViewportParams *viewport , const QString &layer)
{
    Q_UNUSED(layer);
    m_viewport = viewport;
    m_regionList.clear();

    painter->save();
    if ( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
        GeoDataLinearRing &ring = polygon->outerBoundary();
        QVector<GeoDataCoordinates> coordinateList;
        coordinateList.reserve(8);

        coordinateList.append( ring.at( NorthWest ) );
        coordinateList.append( ring.at( SouthWest ) );
        coordinateList.append( ring.at( SouthEast ) );
        coordinateList.append( ring.at( NorthEast ) );

        GeoDataCoordinates northernHandle = ring.at( NorthEast ).interpolate( ring.at( NorthWest ), 0.5 );
        GeoDataCoordinates southernHandle = ring.at( SouthEast ).interpolate( ring.at( SouthWest ), 0.5 );
        // Special case handle position to take tessellation
        // along latitude circles into account
        if (m_overlay->latLonBox().rotation() == 0) {
            northernHandle.setLatitude(ring.at( NorthEast ).latitude());
            southernHandle.setLatitude(ring.at( SouthEast ).latitude());
        }
        coordinateList.append( northernHandle );
        coordinateList.append( southernHandle );

        coordinateList.append( ring.at( NorthEast ).interpolate( ring.at( SouthEast ), 0.5 ) );
        coordinateList.append( ring.at( NorthWest ).interpolate( ring.at( SouthWest ), 0.5 ) );

        m_regionList.reserve(9);
        m_regionList.append( painter->regionFromEllipse( coordinateList.at( NorthWest ), 16, 16 ) );
        m_regionList.append( painter->regionFromEllipse( coordinateList.at( SouthWest ), 16, 16 ) );
        m_regionList.append( painter->regionFromEllipse( coordinateList.at( SouthEast ), 16, 16 ) );
        m_regionList.append( painter->regionFromEllipse( coordinateList.at( NorthEast ), 16, 16 ) );
        m_regionList.append( painter->regionFromEllipse( coordinateList.at( North ), 16, 16 ) );
        m_regionList.append( painter->regionFromEllipse( coordinateList.at( South ), 16, 16 ) );
        m_regionList.append( painter->regionFromEllipse( coordinateList.at( East ),  16, 16 ) );
        m_regionList.append( painter->regionFromEllipse( coordinateList.at( West ),  16, 16 ) );
        m_regionList.append( painter->regionFromPolygon( ring, Qt::OddEvenFill ) );

        // Calculate handle icon orientation due to the projection
        qreal xNW, yNW, xSW, ySW;
        viewport->screenCoordinates(ring.at( NorthWest ), xNW, yNW);
        viewport->screenCoordinates(ring.at( SouthWest ), xSW, ySW);
        qreal westernAngle = qAtan2(ySW - yNW, xSW - xNW) - M_PI/2;
        qreal xNE, yNE, xSE, ySE;
        viewport->screenCoordinates(ring.at( NorthEast ), xNE, yNE);
        viewport->screenCoordinates(ring.at( SouthEast ), xSE, ySE);
        qreal easternAngle = qAtan2(ySE - yNE, xSE - xNE) - M_PI/2;

        painter->setPen( Qt::DashLine );
        painter->setBrush( Qt::NoBrush );
        painter->drawPolygon( ring );

        qreal projectedAngle = 0;

        for( int i = NorthWest; i != Polygon; ++i ) {

            // Assign handle icon orientation due to the projection
            if (i == NorthWest || i == West || i == SouthWest) {
                projectedAngle = westernAngle;
            }
            else if (i == NorthEast || i == East || i == SouthEast) {
                projectedAngle = easternAngle;
            }
            else if (i == North || i == South) {
                projectedAngle = (westernAngle + easternAngle) / 2;
            }
            QTransform trans;
            trans.rotateRadians( projectedAngle );
            if ( m_editStatus == Resize ){
                if( m_hoveredHandle != i ) {
                    painter->drawImage( coordinateList.at( i ),
                                        m_resizeIcons.at( 2*i ).transformed( trans, Qt::SmoothTransformation ) );
                } else {
                    painter->drawImage( coordinateList.at( i ),
                                        m_resizeIcons.at( 2*i + 1 ).transformed( trans, Qt::SmoothTransformation ) );
                }
            } else if ( m_editStatus == Rotate ) {
                if( m_hoveredHandle != i ) {
                    painter->drawImage( coordinateList.at( i ),
                                        m_rotateIcons.at( 2*i ).transformed( trans, Qt::SmoothTransformation ) );
                } else {
                    painter->drawImage( coordinateList.at( i ),
                                        m_rotateIcons.at( 2*i + 1 ).transformed( trans, Qt::SmoothTransformation ) );
                }
            }
        }
    }
    painter->restore();
}

bool GroundOverlayFrame::containsPoint( const QPoint &eventPos ) const
{
    foreach ( const QRegion &region, m_regionList ) {
        if ( region.contains( eventPos ) ) {
            return true;
        }
    }

    // This is a bugfix to handle the events even if they occur outside of this object,
    // so when rotating or resizing the mouseReleaseEvent is handled successfully
    // TODO: maybe find a better way?
    if( m_movedHandle   != NoRegion ||
        m_hoveredHandle != NoRegion ) {
        return true;
    }

    return false;
}

void GroundOverlayFrame::dealWithItemChange( const SceneGraphicsItem *other )
{
    Q_UNUSED( other );
}

void GroundOverlayFrame::move( const GeoDataCoordinates &source, const GeoDataCoordinates &destination )
{
    // not implemented yet
    Q_UNUSED( source );
    Q_UNUSED( destination );
}

bool GroundOverlayFrame::mousePressEvent( QMouseEvent *event )
{
    // React to all ellipse as well as to the polygon.
    for ( int i = 0; i < m_regionList.size(); ++i ) {
        if ( m_regionList.at(i).contains( event->pos() ) ) {
            m_movedHandle = i;

            qreal lon, lat;
            m_viewport->geoCoordinates( event->pos().x(),
                                        event->pos().y(),
                                        lon, lat,
                                        GeoDataCoordinates::Radian );
            m_movedHandleGeoCoordinates.set( lon, lat );
            m_movedHandleScreenCoordinates = event->pos();
            m_previousRotation = m_overlay->latLonBox().rotation();

            if ( m_movedHandle == Polygon ) {
                m_editStatusChangeNeeded = true;
            }

            return true;
        }
    }

    return false;
}

bool GroundOverlayFrame::mouseMoveEvent( QMouseEvent *event )
{
    if ( !m_viewport ) {
        return false;
    }

    // Catch hover events.
    if ( m_movedHandle == NoRegion ) {
        for ( int i = 0; i < m_regionList.size(); ++i ) {
            if ( m_regionList.at(i).contains( event->pos() ) ) {
                if ( i == Polygon ) {
                    setRequest( ChangeCursorOverlayBodyHover );
                } else {
                    setRequest( ChangeCursorOverlayRotateHover );
                }
                m_hoveredHandle = i;
                return true;
            }
        }
        m_hoveredHandle = NoRegion;
        return true;
    } else {
        m_editStatusChangeNeeded = false;
    }

    if ( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        qreal lon, lat;
        m_viewport->geoCoordinates( event->pos().x(),
                                    event->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );

        if ( m_editStatus == Resize ) {

            GeoDataCoordinates coord(lon, lat);
            GeoDataCoordinates rotatedCoord(coord);

            if (m_overlay->latLonBox().rotation() != 0) {
                rotatedCoord = coord.rotateAround(m_overlay->latLonBox().center(),
                                                 -m_overlay->latLonBox().rotation());
            }

            if ( m_movedHandle == NorthWest ) {
                m_overlay->latLonBox().setNorth( rotatedCoord.latitude() );
                m_overlay->latLonBox().setWest( rotatedCoord.longitude() );
            } else if ( m_movedHandle == SouthWest ) {
                m_overlay->latLonBox().setSouth( rotatedCoord.latitude() );
                m_overlay->latLonBox().setWest( rotatedCoord.longitude() );
            } else if ( m_movedHandle == SouthEast ) {
                m_overlay->latLonBox().setSouth( rotatedCoord.latitude() );
                m_overlay->latLonBox().setEast( rotatedCoord.longitude() );
            } else if ( m_movedHandle == NorthEast ) {
                m_overlay->latLonBox().setNorth( rotatedCoord.latitude() );
                m_overlay->latLonBox().setEast( rotatedCoord.longitude() );
            } else if ( m_movedHandle == North ) {
                m_overlay->latLonBox().setNorth( rotatedCoord.latitude() );
            } else if ( m_movedHandle == South ) {
                m_overlay->latLonBox().setSouth( rotatedCoord.latitude() );
            } else if ( m_movedHandle == East ) {
                m_overlay->latLonBox().setEast( rotatedCoord.longitude() );
            } else if ( m_movedHandle == West ) {
                m_overlay->latLonBox().setWest( rotatedCoord.longitude() );
            }

        } else if ( m_editStatus == Rotate ) {
            if ( m_movedHandle != Polygon ) {
                QPoint center = m_regionList.at( Polygon ).boundingRect().center();
                qreal angle1 = qAtan2( event->pos().y() - center.y(),
                                       event->pos().x() - center.x() );
                qreal angle2 = qAtan2( m_movedHandleScreenCoordinates.y() - center.y(),
                                       m_movedHandleScreenCoordinates.x() - center.x() );
                m_overlay->latLonBox().setRotation( angle2 - angle1 + m_previousRotation );
            }
        }

        if ( m_movedHandle == Polygon ) {
            const qreal centerLonDiff = lon - m_movedHandleGeoCoordinates.longitude();
            const qreal centerLatDiff = lat - m_movedHandleGeoCoordinates.latitude();

            m_overlay->latLonBox().setBoundaries( m_overlay->latLonBox().north() + centerLatDiff,
                                                  m_overlay->latLonBox().south() + centerLatDiff,
                                                  m_overlay->latLonBox().east()  + centerLonDiff,
                                                  m_overlay->latLonBox().west()  + centerLonDiff );

            m_movedHandleGeoCoordinates.set( lon, lat );
        }

        update();
        return true;
    }
    return false;
}

bool GroundOverlayFrame::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event );

    m_movedHandle = NoRegion;
    m_textureLayer->reset();

    if( m_editStatusChangeNeeded ) {
        if( m_editStatus == Resize ) {
            m_editStatus = Rotate;
        } else {
            m_editStatus = Resize;
        }
    }

    return true;
}

void GroundOverlayFrame::update()
{
    GeoDataLatLonBox overlayLatLonBox = m_overlay->latLonBox();
    GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( placemark()->geometry() );
    poly->outerBoundary().clear();

    GeoDataCoordinates rotatedCoord;

    GeoDataCoordinates northWest(overlayLatLonBox.west(), overlayLatLonBox.north());
    rotatedCoord = northWest.rotateAround(overlayLatLonBox.center(), overlayLatLonBox.rotation());
    poly->outerBoundary().append( rotatedCoord );

    GeoDataCoordinates southWest(overlayLatLonBox.west(), overlayLatLonBox.south());
    rotatedCoord = southWest.rotateAround(overlayLatLonBox.center(), overlayLatLonBox.rotation());
    poly->outerBoundary().append( rotatedCoord );

    GeoDataCoordinates southEast(overlayLatLonBox.east(), overlayLatLonBox.south());
    rotatedCoord = southEast.rotateAround(overlayLatLonBox.center(), overlayLatLonBox.rotation());
    poly->outerBoundary().append( rotatedCoord );

    GeoDataCoordinates northEast(overlayLatLonBox.east(), overlayLatLonBox.north());
    rotatedCoord = northEast.rotateAround(overlayLatLonBox.center(), overlayLatLonBox.rotation());
    poly->outerBoundary().append( rotatedCoord );
}

void GroundOverlayFrame::dealWithStateChange( SceneGraphicsItem::ActionState previousState )
{
    Q_UNUSED( previousState );
}

const char *GroundOverlayFrame::graphicType() const
{
    return SceneGraphicsTypes::SceneGraphicGroundOverlay;
}


}
