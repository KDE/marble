//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn  <rahn@kde.org>
//

#include "MarblePhysics.h"

#include "Quaternion.h"
#include "MarbleWidget.h"
#include "MarbleDebug.h"
#include "GeoDataLineString.h"
#include "ViewportParams.h"
#include "AbstractProjection.h"

#include <QtCore/QTimeLine>

namespace Marble
{

class MarblePhysicsPrivate {
public:
    MarbleWidget *const m_widget;

    GeoDataLookAt m_source;

    GeoDataLookAt m_target;

    FlyToMode m_mode;

    QTimeLine m_timeline;

    qreal m_planetRadius;

    MarblePhysicsPrivate( MarbleWidget *widget )
        : m_widget( widget ),
          m_mode( Instant ),
          m_planetRadius( EARTH_RADIUS )
    {
        m_timeline.setDuration(2000);
        m_timeline.setCurveShape( QTimeLine::EaseInOutCurve );
    }

    void suggestedPos(qreal t, qreal &lon, qreal &lat) const
    {
        Q_ASSERT( 0 <= t && t <= 1.0 );

        Quaternion  itpos;    
        GeoDataCoordinates sourcePosition(m_source.longitude(), m_source.latitude());
        GeoDataCoordinates targetPosition(m_target.longitude(), m_target.latitude());

        // Spherical interpolation for current position between source position
        // and target position. We can't use Nlerp here, as the "t-velocity" needs to be constant.
        itpos.slerp( sourcePosition.quaternion(), targetPosition.quaternion(), t );
        itpos.getSpherical( lon, lat );
    }

    qreal totalDistance() const
    {
        GeoDataLineString measure;
        GeoDataCoordinates sourcePosition(m_source.longitude(), m_source.latitude());
        GeoDataCoordinates targetPosition(m_target.longitude(), m_target.latitude());
        measure << sourcePosition << targetPosition;
        return measure.length(m_planetRadius);
    }

    qreal suggestedRange(qreal t) const
    {
        Q_ASSERT( m_mode == Linear || m_mode == Jump);
        Q_ASSERT( 0 <= t && t <= 1.0 );

        if (m_mode == Linear) {
            qreal in = m_source.range();
            qreal out = m_target.range();

            return in + t * (out-in);
        }
        else if (m_mode == Jump) {
            qreal jumpDuration = m_timeline.duration();

            // Purely cinematic approach to calculate the jump path        
            qreal g = qMin(m_source.range(), m_target.range()); // Min altitude
            qreal k = qMax(m_source.range(), m_target.range()); // Base altitude
            qreal d = t > 0.5 ? m_source.range() - g : m_target.range() - g; // Base difference
            qreal c = d * 2 * qAbs(t - 0.5); // Correction factor
            qreal h = qMin(1000*3000.0, totalDistance() / 2.0); // Jump height

            // Parameters for the parabolic function that has the maximum at
            // the point H ( 0.5 * m_jumpDuration, g + h )
            qreal a = - h / ( (qreal)( 0.25 * jumpDuration * jumpDuration ) );
            qreal b = 2.0 * h / (qreal)( 0.5 * jumpDuration );

            qreal x = jumpDuration * t;
            qreal y = ( a * x + b ) * x + k - c;       // Parabolic function

            return y;
        }
        else {
            qWarning("Unhandled FlyTo mode, no camera distance interpolation.");
            return m_target.range();
        }
    }
};


MarblePhysics::MarblePhysics( MarbleWidget *widget )
    : QObject( widget ),
      d( new MarblePhysicsPrivate( widget ) )
{
    connect( &d->m_timeline, SIGNAL( valueChanged( qreal ) ),
             this, SLOT( updateProgress( qreal ) ) );
    connect( &d->m_timeline, SIGNAL( finished() ),
             this, SLOT( startStillMode() ) );
}

MarblePhysics::~MarblePhysics()
{
    delete d;
}

void MarblePhysics::flyTo( const GeoDataLookAt &target, FlyToMode mode )
{
    d->m_timeline.stop();
    d->m_source = d->m_widget->lookAt();
    d->m_target = target;
    const ViewportParams *viewport = d->m_widget->viewport();

    FlyToMode effectiveMode = mode;
    qreal x(0), y(0);
    bool globeHidesPoint(false);
    bool onScreen = viewport->currentProjection()->screenCoordinates( target.coordinates(),
                                                                      viewport,
                                                                      x, y,
                                                                      globeHidesPoint );
    bool invisible = globeHidesPoint || !onScreen;

    if (effectiveMode == Automatic)
    {
        bool zoom = qAbs(d->m_source.range()-target.range()) > 10;

        if ( (invisible || zoom) ) {
            effectiveMode = Jump;
        } else {
            effectiveMode = Linear;
        }
    }

    d->m_mode = effectiveMode;

    switch(effectiveMode)
    {
    case Instant:
        d->m_widget->flyTo( target, Instant );
        return;
        break;
    case Linear:
        d->m_timeline.setDuration(300);
        d->m_timeline.setCurveShape( QTimeLine::EaseOutCurve );
        break;
    case Jump:
        {
            qreal duration = invisible ? 2000 : 1000;
            d->m_timeline.setDuration(duration);
            d->m_timeline.setCurveShape( QTimeLine::EaseInOutCurve );
        }
        break;
    case Automatic:
        Q_ASSERT(false);
        break;
    }

    d->m_timeline.start();
}

void MarblePhysics::updateProgress(qreal progress)
{
    Q_ASSERT(d->m_mode != Instant);
    Q_ASSERT(d->m_mode != Automatic);

    if (progress >= 1.0)
    {
        d->m_widget->flyTo( d->m_target, Instant );
        d->m_widget->setViewContext( Marble::Still );
        return;
    }

    Q_ASSERT(progress >= 0.0 && progress < 1.0);
    qreal lon(0.0), lat(0.0);
    d->suggestedPos(progress, lon, lat);
    qreal range = d->suggestedRange(progress);

    GeoDataLookAt intermediate;
    intermediate.setLongitude(lon, GeoDataCoordinates::Radian);
    intermediate.setLatitude(lat, GeoDataCoordinates::Radian);
    intermediate.setAltitude(0.0);
    intermediate.setRange(range);

    d->m_widget->setViewContext( Marble::Animation );
    d->m_widget->flyTo( intermediate, Instant );
}

void MarblePhysics::startStillMode()
{
    d->m_widget->setViewContext( Marble::Still );
}

}

#include "MarblePhysics.moc"
