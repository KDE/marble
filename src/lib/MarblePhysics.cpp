//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn  <rahn@kde.org>
//

#include <QtCore/QDebug>

// Own
#include "MarblePhysics.h"


MarblePhysics::MarblePhysics()
    : m_gravity( 9.81 ),
      m_timeLine( new QTimeLine( 2000 ) )
{
    m_timeLine->setFrameRange( 0, 500 );
    m_timeLine->setCurveShape( QTimeLine::EaseInOutCurve );
    m_timeLine->setUpdateInterval( 0 );
    connect( m_timeLine, SIGNAL( valueChanged( qreal ) ), SIGNAL( valueChanged( qreal ) ) );
}

MarblePhysics::~MarblePhysics()
{
    delete m_timeLine;
}

GeoDataPoint MarblePhysics::suggestedPosition() const
{
    double lon, lat;
    Quaternion  itpos;

    double t = m_timeLine->currentValue();

    itpos.slerp( m_sourcePosition.quaternion(), m_targetPosition.quaternion(), t );
    itpos.getSpherical( lon, lat );

    double g = m_sourcePosition.altitude();
    double h = 3000.0;

    double a = - h / ( 1000.0 * 1000.0 );
    double b = 2.0 * h / 1000.0;

    double x = 2000.0 * t;

    double y = a * x * x + b * x + g;

    return GeoDataPoint( lon, lat, y );
}

void MarblePhysics::jumpTo( const GeoDataPoint &targetPosition )
{
    m_targetPosition = targetPosition;
    m_timeLine->start();
}

void MarblePhysics::setCurrentPosition( const GeoDataPoint &sourcePosition )
{
    m_timeLine->stop();
    m_sourcePosition = sourcePosition;
}

#include "MarblePhysics.moc"
