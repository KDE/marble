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
#include "GeoDataTypes.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "SceneGraphicsTypes.h"


namespace Marble
{

GroundOverlayFrame::GroundOverlayFrame( GeoDataPlacemark *placemark,
                                        GeoDataGroundOverlay *overlay,
                                        TextureLayer *textureLayer ) :
    SceneGraphicsItem( placemark ),
    m_overlay( overlay ),
    m_textureLayer( textureLayer ),
    m_movedPoint( -1 ),
    m_viewport( 0 )
{
    update();
}

void GroundOverlayFrame::paint(GeoPainter *painter, const ViewportParams *viewport )
{
    m_viewport = viewport;
    m_regionList.clear();

    painter->save();
    painter->setBrush( Oxygen::aluminumGray4 );
    if ( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark()->geometry() );
        GeoDataLinearRing &ring = polygon->outerBoundary();
        for ( int i = 0; i < ring.size(); ++i ) {
            m_regionList.append( painter->regionFromEllipse( ring.at(i), 10, 10 ) );
        }
        m_regionList.append( painter->regionFromPolygon( ring, Qt::OddEvenFill ) );
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

    return false;
}

void GroundOverlayFrame::dealWithItemChange( const SceneGraphicsItem *other )
{
    Q_UNUSED( other );
}

void GroundOverlayFrame::move( const GeoDataCoordinates &source, const GeoDataCoordinates &destination )
{
    // not implemented yet
}

bool GroundOverlayFrame::mousePressEvent( QMouseEvent *event )
{
    // React to all ellipse as well as to the polygon.
    for ( int i = 0; i < m_regionList.size(); ++i ) {
        if ( m_regionList.at(i).contains( event->pos() ) ) {
            m_movedPoint = i;

            qreal lon, lat;
            m_viewport->geoCoordinates( event->pos().x(),
                                        event->pos().y(),
                                        lon, lat,
                                        GeoDataCoordinates::Radian );
            m_movedPointCoordinates.set( lon, lat );

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
    if ( m_movedPoint < 0 ) {
        return true;
    }

    if ( placemark()->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        qreal lon, lat;
        m_viewport->geoCoordinates( event->pos().x(),
                                    event->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );

        qreal rotatedLon;
        qreal rotatedLat;
        rotateAroundCenter( lon, lat, rotatedLon, rotatedLat, m_overlay->latLonBox(), true );

        if ( m_movedPoint == NorthWest ) {
            m_overlay->latLonBox().setNorth( rotatedLat );
            m_overlay->latLonBox().setWest( rotatedLon );
        } else if ( m_movedPoint == SouthWest ) {
            m_overlay->latLonBox().setSouth( rotatedLat );
            m_overlay->latLonBox().setWest( rotatedLon );
        } else if ( m_movedPoint == SouthEast ) {
            m_overlay->latLonBox().setSouth( rotatedLat );
            m_overlay->latLonBox().setEast( rotatedLon );
        } else if ( m_movedPoint == NorthEast ) {
            m_overlay->latLonBox().setNorth( rotatedLat );
            m_overlay->latLonBox().setEast( rotatedLon );
        } else if ( m_movedPoint == Polygon ) {

           qreal centerLonDiff = lon - m_movedPointCoordinates.longitude();
           qreal centerLatDiff = lat - m_movedPointCoordinates.latitude();

           m_overlay->latLonBox().setBoundaries( m_overlay->latLonBox().north() + centerLatDiff,
                                                 m_overlay->latLonBox().south() + centerLatDiff,
                                                 m_overlay->latLonBox().east() + centerLonDiff,
                                                 m_overlay->latLonBox().west() + centerLonDiff );

           m_movedPointCoordinates.set( lon, lat );
        }

        update();
        return true;
    }
    return false;
}

bool GroundOverlayFrame::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event );

    m_movedPoint = -1;
    m_textureLayer->reset();

    return true;
}

void GroundOverlayFrame::update()
{
    GeoDataLatLonBox overlayLatLonBox = m_overlay->latLonBox();

    GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( placemark()->geometry() );

    poly->outerBoundary().clear();

    qreal rotatedLon;
    qreal rotatedLat;

    rotateAroundCenter( overlayLatLonBox.west(), overlayLatLonBox.north(), rotatedLon, rotatedLat, overlayLatLonBox );
    poly->outerBoundary().append( GeoDataCoordinates( rotatedLon, rotatedLat ) );

    rotateAroundCenter( overlayLatLonBox.west(), overlayLatLonBox.south(), rotatedLon, rotatedLat, overlayLatLonBox );
    poly->outerBoundary().append( GeoDataCoordinates( rotatedLon, rotatedLat ) );

    rotateAroundCenter( overlayLatLonBox.east(), overlayLatLonBox.south(), rotatedLon, rotatedLat, overlayLatLonBox );
    poly->outerBoundary().append( GeoDataCoordinates( rotatedLon, rotatedLat ) );

    rotateAroundCenter( overlayLatLonBox.east(), overlayLatLonBox.north(), rotatedLon, rotatedLat, overlayLatLonBox );
    poly->outerBoundary().append( GeoDataCoordinates( rotatedLon, rotatedLat ) );
}

void GroundOverlayFrame::rotateAroundCenter( qreal lon, qreal lat, qreal &rotatedLon, qreal &rotatedLat, GeoDataLatLonBox &box, bool inverse )
{
    const qreal angle = ( inverse ? ( -1 ) : 1 ) * box.rotation();
    const qreal sinRotation = sin( angle );
    const qreal cosRotation = cos( angle );

    const qreal centerLat = box.center().latitude();
    qreal centerLon = box.center().longitude();

    if ( box.crossesDateLine() ) {
        if ( lon < 0 && centerLon > 0 ) {
            centerLon -= 2 * M_PI;
        }
        if ( lon > 0 && centerLon < 0  ) {
            centerLon += 2 * M_PI;
        }
        if ( box.west() > 0 && box.east() > 0 && box.west() > box.east() && lon > 0 && lon < box.west() ) {
            if ( ! ( lon < box.west() && lon > box.toCircumscribedRectangle().west() ) ) {
               centerLon -= 2 * M_PI;
            }
        }
    }

    rotatedLon = ( lon - centerLon ) * cosRotation - ( lat - centerLat ) * sinRotation + centerLon;
    rotatedLat = ( lon - centerLon ) * sinRotation + ( lat - centerLat ) * cosRotation + centerLat;

    GeoDataCoordinates::normalizeLonLat( rotatedLon, rotatedLat );
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
