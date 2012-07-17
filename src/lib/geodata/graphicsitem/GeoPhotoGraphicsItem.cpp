//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GeoPhotoGraphicsItem.h"

#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QtGui/QImageReader>

#include <QtCore/QDebug>

namespace Marble
{

void GeoPhotoGraphicsItem::setPhoto( const QImage &photo )
{
    m_photo = photo;
}

QImage GeoPhotoGraphicsItem::photo() const
{
    return m_photo;
}

void GeoPhotoGraphicsItem::setPhotoFile( const QString &filename )
{
    m_photoPath = filename;
}

QString GeoPhotoGraphicsItem::photoPath() const
{
    return m_photoPath;
}

void GeoPhotoGraphicsItem::paint( GeoPainter* painter, ViewportParams* viewport,
                                  const QString& renderPos, GeoSceneLayer* layer )
{   
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    /** @todo FIXME: need access to MarbleModel here ideally */
    qreal const planetRadius = EARTH_RADIUS;
    /** @todo: Taken from MarbleWidgetPrivate */
    qreal const viewAngle = 110.0;
    qreal const cameraDistance = 1000 * planetRadius * 0.4 / viewport->radius() / tan( 0.5 * viewAngle * DEG2RAD );
    /** @todo: Extract the distance between camera and photo from kml */
    qreal const near = 200.0;

    /* The code below loads the image lazily (only
     * when it will actually be displayed). Once it was
     * loaded but moves out of the viewport, it is unloaded
     * again. Otherwise memory consumption gets quite high
     * for a large set of photos
     */
    bool unloadImage = true;

    if ( cameraDistance > m_point.altitude() ) {
        QSizeF size;
        if ( m_photo.isNull() && !m_photoPath.isEmpty() ) {
            QImageReader reader( m_photoPath );
            size = reader.size(); // Image is not loaded yet
        } else {
            size = m_photo.size(); // Image loaded or invalid
        }
        size *= (m_point.altitude() + near) / cameraDistance;
        if ( size.width() * size.height() > 256 ) {
            // The image gets displayed if the observer is above it and if it covers a certain minimum area
            qreal x(0.0), y( 0.0 );
            viewport->screenCoordinates( m_point, x, y );
            QRectF position( QPointF( x, y ), size );
            position.moveCenter( QPointF( x, y ) );

            QRectF displayed = position & QRectF( QPointF( 0, 0 ), viewport->size() );
            if( !displayed.isEmpty() ) {
                if ( m_photo.isNull() ) {
                    /** @todo: Load in a thread */
                    m_photo = QImage( m_photoPath );
                }
                unloadImage = false;
                painter->drawImage( position, m_photo );
            }
        }
    }

    if ( unloadImage && !m_photoPath.isEmpty() ) {
        // No unloading if no path is known
        m_photo = QImage();
    }
}

GeoDataLatLonAltBox& GeoPhotoGraphicsItem::latLonAltBox() const
{
    return m_point.latLonAltBox();
}

void GeoPhotoGraphicsItem::setPoint( const GeoDataPoint &point )
{
    m_point = point;
}

GeoDataPoint GeoPhotoGraphicsItem::point() const
{
    return m_point;
}

}
