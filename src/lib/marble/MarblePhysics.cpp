// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
//

#include "MarblePhysics.h"

#include "GeoDataLineString.h"
#include "GeoDataLookAt.h"
#include "MarbleAbstractPresenter.h"
#include "MarbleDebug.h"
#include "Quaternion.h"
#include "ViewportParams.h"

#include <QTimeLine>

namespace Marble
{

class MarblePhysicsPrivate
{
public:
    MarbleAbstractPresenter *const m_presenter;

    GeoDataLookAt m_source;

    GeoDataLookAt m_target;

    FlyToMode m_mode;

    QTimeLine m_timeline;

    qreal m_planetRadius;

    explicit MarblePhysicsPrivate(MarbleAbstractPresenter *presenter)
        : m_presenter(presenter)
        , m_mode(Instant)
        , m_planetRadius(EARTH_RADIUS)
    {
        m_timeline.setDuration(2000);
        m_timeline.setEasingCurve(QEasingCurve::InOutSine);
    }

    qreal suggestedRange(qreal t) const
    {
        Q_ASSERT(m_mode == Linear || m_mode == Jump);
        Q_ASSERT(0 <= t && t <= 1.0);

        if (m_mode == Linear) {
            qreal in = m_source.range();
            qreal out = m_target.range();

            return in + t * (out - in);
        } else if (m_mode == Jump) {
            qreal jumpDuration = m_timeline.duration();

            // Purely cinematic approach to calculate the jump path
            const qreal totalDistance = m_planetRadius * m_source.coordinates().sphericalDistanceTo(m_target.coordinates());
            qreal g = qMin(m_source.range(), m_target.range()); // Min altitude
            qreal k = qMax(m_source.range(), m_target.range()); // Base altitude
            qreal d = t > 0.5 ? m_source.range() - g : m_target.range() - g; // Base difference
            qreal c = d * 2 * qAbs(t - 0.5); // Correction factor
            qreal h = qMin(1000 * 3000.0, totalDistance / 2.0); // Jump height

            // Parameters for the parabolic function that has the maximum at
            // the point H ( 0.5 * m_jumpDuration, g + h )
            qreal a = -h / ((qreal)(0.25 * jumpDuration * jumpDuration));
            qreal b = 2.0 * h / (qreal)(0.5 * jumpDuration);

            qreal x = jumpDuration * t;
            qreal y = (a * x + b) * x + k - c; // Parabolic function

            return y;
        } else {
            qWarning("Unhandled FlyTo mode, no camera distance interpolation.");
            return m_target.range();
        }
    }
};

MarblePhysics::MarblePhysics(MarbleAbstractPresenter *presenter)
    : QObject(presenter)
    , d(new MarblePhysicsPrivate(presenter))
{
    connect(&d->m_timeline, &QTimeLine::valueChanged, this, &MarblePhysics::updateProgress);
    connect(&d->m_timeline, &QTimeLine::finished, this, &MarblePhysics::startStillMode);
}

MarblePhysics::~MarblePhysics()
{
    delete d;
}

void MarblePhysics::flyTo(const GeoDataLookAt &target, FlyToMode mode)
{
    d->m_timeline.stop();
    d->m_source = d->m_presenter->lookAt();
    d->m_target = target;
    const ViewportParams *viewport = d->m_presenter->viewport();

    FlyToMode effectiveMode = mode;
    qreal x(0), y(0);
    bool globeHidesPoint(false);
    bool onScreen = viewport->screenCoordinates(target.coordinates(), x, y, globeHidesPoint);
    bool invisible = globeHidesPoint || !onScreen;

    if (effectiveMode == Automatic) {
        bool zoom = qAbs(d->m_source.range() - target.range()) > 10;

        if ((invisible || zoom)) {
            effectiveMode = Jump;
        } else {
            effectiveMode = Linear;
        }
    }

    d->m_mode = effectiveMode;

    switch (effectiveMode) {
    case Instant:
        d->m_presenter->flyTo(target, Instant);
        return;
    case Linear:
        d->m_timeline.setDuration(300);
        d->m_timeline.setEasingCurve(QEasingCurve::OutCurve);
        break;
    case Jump: {
        qreal duration = invisible ? 2000 : 1000;
        d->m_timeline.setDuration(duration);
        d->m_timeline.setEasingCurve(QEasingCurve::InOutSine);
    } break;
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

    if (progress >= 1.0) {
        d->m_presenter->flyTo(d->m_target, Instant);
        d->m_presenter->setViewContext(Marble::Still);
        return;
    }

    Q_ASSERT(progress >= 0.0 && progress < 1.0);
    const GeoDataCoordinates interpolated = d->m_source.coordinates().interpolate(d->m_target.coordinates(), progress);
    qreal range = d->suggestedRange(progress);

    GeoDataLookAt intermediate;
    intermediate.setCoordinates(interpolated);
    intermediate.setRange(range);

    d->m_presenter->setViewContext(Marble::Animation);
    d->m_presenter->flyTo(intermediate, Instant);
}

void MarblePhysics::startStillMode()
{
    d->m_presenter->setViewContext(Marble::Still);
}

}

#include "moc_MarblePhysics.cpp"
