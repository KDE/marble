//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


// Own
#include "GeoDataContainer.h"

// Qt
#include <QtGui/QImage>

// Marble
#include "GeoDataFeature.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"

namespace Marble
{

class GeoDataContainerPrivate
{
  public:
    GeoDataContainerPrivate()
    {
    }

    ~GeoDataContainerPrivate()
    {
        qDeleteAll(m_features);
    }

    /// The vector holding all the features in the container.
    QVector < GeoDataFeature* >  m_features;
};

GeoDataContainer::GeoDataContainer( GeoDataObject *parent )
    : GeoDataFeature( parent ),
    d( new GeoDataContainerPrivate() )
{
}

GeoDataContainer::~GeoDataContainer()
{
    delete d;
}


QVector<GeoDataFolder*> GeoDataContainer::folders() const
{
    QVector<GeoDataFolder*> results;

    QVector<GeoDataFeature*>::const_iterator it = d->m_features.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = d->m_features.constEnd();

    for (; it != end; ++it) {
        GeoDataFeature* feature = *it;

        if ( dynamic_cast<GeoDataFolder*>( feature ) )
            results.append( static_cast<GeoDataFolder*>( feature ) );
    }

    return results;
}

QVector<GeoDataPlacemark*> GeoDataContainer::placemarks() const
{
    QVector<GeoDataPlacemark*> results;

    QVector<GeoDataFeature*>::const_iterator it = d->m_features.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = d->m_features.constEnd();

    for (; it != end; ++it) {
        GeoDataFeature* feature = *it;
        if ( dynamic_cast<GeoDataPlacemark*>( feature ) )
            results.append( static_cast<GeoDataPlacemark*>( feature ) );
    }

    return results;
}

QVector<GeoDataFeature*> GeoDataContainer::features() const
{
    return d->m_features;
}

void GeoDataContainer::addFeature( GeoDataFeature* feature, bool setChild )
{
    if( setChild ) feature->setParent( this );
    d->m_features.append( feature );
}

void GeoDataContainer::removeFeature(GeoDataFeature* feature )
{
    int pos = 0;
    if( pos = d->m_features.indexOf( feature ) && pos >= 0 ) {
        feature->setParent( 0 );
        d->m_features.remove( pos );
    }
}

GeoDataObject* GeoDataContainer::child( int pos )
{
    return d->m_features.value( pos );
}

int GeoDataContainer::childPosition( GeoDataObject *child )
{
    return d->m_features.indexOf( static_cast<GeoDataFeature*>( child ) );
}

int GeoDataContainer::childCount()
{
    return d->m_features.count();
}

void GeoDataContainer::pack( QDataStream& stream ) const
{
    GeoDataFeature::pack( stream );

    stream << d->m_features.count();

    for ( QVector <GeoDataFeature*>::const_iterator iterator = d->m_features.constBegin();
          iterator != d->m_features.constEnd();
          ++iterator )
    {
        const GeoDataFeature& feature = * ( *iterator );
        stream << feature.featureId();
        feature.pack( stream );
    }
}

void GeoDataContainer::unpack( QDataStream& stream )
{
    GeoDataFeature::unpack( stream );

    int count;
    stream >> count;

    for ( int i = 0; i < count; ++i ) {
        int featureId;
        stream >> featureId;
        switch( featureId ) {
            case GeoDataDocumentId:
                /* not usable!!!! */ break;
            case GeoDataFolderId:
                {
                GeoDataFolder* folder = new GeoDataFolder( this );
                folder->unpack( stream );
                d->m_features.append( folder );
                }
                break;
            case GeoDataPlacemarkId:
                {
                GeoDataPlacemark* placemark = new GeoDataPlacemark( this );
                placemark->unpack( stream );
                d->m_features.append( placemark );
                }
                break;
            case GeoDataNetworkLinkId:
                break;
            case GeoDataScreenOverlayId:
                break;
            case GeoDataGroundOverlayId:
                break;
            default: break;
        };
    }
}


#if 0   // Shouldn't be here at all
PlacemarkContainer& GeoDataContainer::activePlacemarkContainer( const ViewParams& viewParams )
{
    switch( viewParams.m_projection ) {
        case Spherical:
            return sphericalActivePlacemarkContainer(viewParams);
            break;
        case Equirectangular:
            return rectangularActivePlacemarkContainer(viewParams);
            break;
    }
    //
    // Return at least something...
    //
    return sphericalActivePlacemarkContainer(viewParams);
}


PlacemarkContainer& GeoDataContainer::sphericalActivePlacemarkContainer( const ViewParams& viewParams )
{
    m_activePlacemarkContainer.clear ();

    int x = 0;
    int y = 0;

    int imgwidth = viewParams.m_canvasImage->width();
    int imgheight = viewParams.m_canvasImage->height();

    Quaternion  invplanetAxis = viewParams.m_planetAxis.inverse();
    Quaternion  qpos;

    QVector < GeoDataPlacemark* >::const_iterator  it;
    for ( it = m_placemarkVector.constBegin();
          it != m_placemarkVector.constEnd();
          it++ )
    {
        GeoDataPlacemark* placemark = *it;
        qpos = placemark->coordinate().quaternion();

        qpos.rotateAroundAxis(invplanetAxis);

        if ( qpos.v[Q_Z] > 0 ) {

            x = (int)(imgwidth  / 2 + viewParams.m_radius * qpos.v[Q_X]);
            y = (int)(imgheight / 2 + viewParams.m_radius * qpos.v[Q_Y]);

            // Don't process placemarks if they are outside the screen area
            if ( x >= 0 && x < imgwidth && y >= 0 && y < imgheight ) {
                m_activePlacemarkContainer.append( placemark );
            }
        }
    }

    qDebug("GeoDataDocument::activePlacemarkContainer (). Placemarks count: %d", m_activePlacemarkContainer.count());
    return m_activePlacemarkContainer;
}

PlacemarkContainer& GeoDataContainer::rectangularActivePlacemarkContainer( const ViewParams& viewParams )
{
    m_activePlacemarkContainer.clear ();

    int x = 0;
    int y = 0;

    int imgwidth = viewParams.m_canvasImage->width();
    int imgheight = viewParams.m_canvasImage->height();

    Quaternion  invplanetAxis = viewParams.m_planetAxis.inverse();
    Quaternion  qpos;

    float const centerLat =  viewParams.m_planetAxis.pitch();
    float const centerLon = -viewParams.m_planetAxis.yaw();

    QVector < GeoDataPlacemark* >::const_iterator  it;
    for ( it = m_placemarkVector.constBegin();
          it != m_placemarkVector.constEnd();
          it++ )
    {
        GeoDataPlacemark* placemark = *it;
        qpos = placemark->coordinate().quaternion();

        qreal xyFactor = (float)(2 * viewParams.m_radius) / M_PI;

        qreal degX;
        qreal degY;
        qpos.getSpherical(degX,degY);

        x = (int)(imgwidth  / 2 + xyFactor * (degX + centerLon));
        y = (int)(imgheight / 2 + xyFactor * (degY + centerLat));

        // Don't process placemarks if they are outside the screen area
        if ( ( x >= 0 && x < imgwidth || x+4*viewParams.m_radius < imgwidth || x-4*viewParams.m_radius >= 0 )  && y >= 0 && y < imgheight ) {
            m_activePlacemarkContainer.append( placemark );
        }
    }

    qDebug("GeoDataDocument::activePlacemarkContainer (). Placemarks count: %d", m_activePlacemarkContainer.count());
    return m_activePlacemarkContainer;
}
#endif

}
