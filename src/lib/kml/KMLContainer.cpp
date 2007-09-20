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

#include "KMLContainer.h"
#include "KMLPlaceMark.h"
#include "ViewParams.h"

KMLContainer::KMLContainer()
{
}

KMLContainer::~KMLContainer()
{
    qDebug("KMLContainer::~KMLContainer(). Object count: %d", m_placemarkVector.count());

    foreach ( KMLPlaceMark* placemark, m_placemarkVector ) {
        delete placemark;
    }
}

void KMLContainer::addPlaceMark( KMLPlaceMark* placemark )
{
    m_placemarkVector.append( placemark );
}

PlaceMarkContainer& KMLContainer::activePlaceMarkContainer( const ViewParams& viewParams )
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

PlaceMarkContainer& KMLContainer::sphericalActivePlaceMarkContainer( const ViewParams& viewParams )
{
    m_activePlaceMarkContainer.clear ();

    int x = 0;
    int y = 0;

    int imgwidth = viewParams.m_canvasImage->width();
    int imgheight = viewParams.m_canvasImage->height();

    Quaternion  invplanetAxis = viewParams.m_planetAxis.inverse();
    Quaternion  qpos;

    QVector < KMLPlaceMark* >::const_iterator  it;
    for ( it = m_placemarkVector.constBegin();
          it != m_placemarkVector.constEnd();
          it++ )
    {
        KMLPlaceMark* placemark = *it;
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

    qDebug("KMLDocument::activePlaceMarkContainer (). PlaceMarks count: %d", m_activePlaceMarkContainer.count());
    return m_activePlaceMarkContainer;
}

PlaceMarkContainer& KMLContainer::rectangularActivePlaceMarkContainer( const ViewParams& viewParams )
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

    QVector < KMLPlaceMark* >::const_iterator  it;
    for ( it = m_placemarkVector.constBegin();
          it != m_placemarkVector.constEnd();
          it++ )
    {
        KMLPlaceMark* placemark = *it;
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

    qDebug("KMLDocument::activePlaceMarkContainer (). PlaceMarks count: %d", m_activePlaceMarkContainer.count());
    return m_activePlaceMarkContainer;
}

void KMLContainer::pack( QDataStream& stream ) const
{
    KMLFeature::pack( stream );

    stream << m_placemarkVector.count();

    for ( QVector <KMLPlaceMark*>::const_iterator iterator = m_placemarkVector.constBegin();
          iterator != m_placemarkVector.end();
          iterator++ )
    {
        const KMLPlaceMark& placemark = * ( *iterator );
        placemark.pack( stream );
    }
}

void KMLContainer::unpack( QDataStream& stream )
{
    KMLFeature::unpack( stream );

    int count;
    stream >> count;

    for ( int i = 0; i < count; ++i ) {
        KMLPlaceMark* placemark = new KMLPlaceMark();
        placemark->unpack( stream );

        m_placemarkVector.append( placemark );
    }
}
