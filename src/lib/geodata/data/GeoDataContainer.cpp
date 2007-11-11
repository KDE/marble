//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#include <QtGui/QImage>

#include "GeoDataContainer.h"
#include "GeoDataFeature.h"
#include "ViewParams.h"

GeoDataContainer::GeoDataContainer()
{
}

GeoDataContainer::~GeoDataContainer()
{
    qDebug("GeoDataContainer::~GeoDataContainer(). Object count: %d", m_featureVector.count());

    foreach ( GeoDataFeature* feature, m_featureVector ) {
        delete feature;
    }
}

void GeoDataContainer::addFeature( GeoDataFeature* feature )
{
    m_featureVector.append( feature );
}


void GeoDataContainer::pack( QDataStream& stream ) const
{
    GeoDataFeature::pack( stream );

    stream << m_featureVector.count();

    for ( QVector <GeoDataFeature*>::const_iterator iterator = m_featureVector.constBegin();
          iterator != m_featureVector.end();
          iterator++ )
    {
        const GeoDataFeature& feature = * ( *iterator );
        feature.pack( stream );
    }
}

void GeoDataContainer::unpack( QDataStream& stream )
{
    GeoDataFeature::unpack( stream );

    int count;
    stream >> count;

    for ( int i = 0; i < count; ++i ) {
        GeoDataFeature* feature = new GeoDataFeature();
        feature->unpack( stream );

        m_featureVector.append( feature );
    }
}


#if 0   // Shouldn't be here at all
PlaceMarkContainer& GeoDataContainer::activePlaceMarkContainer( const ViewParams& viewParams )
{
    switch( viewParams.m_projection ) {
        case Spherical:
            return sphericalActivePlaceMarkContainer(viewParams);
            break;
        case Equirectangular:
            return rectangularActivePlaceMarkContainer(viewParams);
            break;
    }
    //
    // Return at least something...
    //
    return sphericalActivePlaceMarkContainer(viewParams);
}


PlaceMarkContainer& GeoDataContainer::sphericalActivePlaceMarkContainer( const ViewParams& viewParams )
{
    m_activePlaceMarkContainer.clear ();

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
                m_activePlaceMarkContainer.append( placemark );
            }
        }
    }

    qDebug("GeoDataDocument::activePlaceMarkContainer (). PlaceMarks count: %d", m_activePlaceMarkContainer.count());
    return m_activePlaceMarkContainer;
}

PlaceMarkContainer& GeoDataContainer::rectangularActivePlaceMarkContainer( const ViewParams& viewParams )
{
    m_activePlaceMarkContainer.clear ();

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

        double xyFactor = (float)(2 * viewParams.m_radius) / M_PI;

        double degX;
        double degY;
        qpos.getSpherical(degX,degY);

        x = (int)(imgwidth  / 2 + xyFactor * (degX + centerLon));
        y = (int)(imgheight / 2 + xyFactor * (degY + centerLat));

        // Don't process placemarks if they are outside the screen area
        if ( ( x >= 0 && x < imgwidth || x+4*viewParams.m_radius < imgwidth || x-4*viewParams.m_radius >= 0 )  && y >= 0 && y < imgheight ) {
            m_activePlaceMarkContainer.append( placemark );
        }
    }

    qDebug("GeoDataDocument::activePlaceMarkContainer (). PlaceMarks count: %d", m_activePlaceMarkContainer.count());
    return m_activePlaceMarkContainer;
}
#endif
