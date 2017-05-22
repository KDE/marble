/*
  This file is part of the Ofi Labs X2 project.

  Copyright (C) 2010 Ariya Hidayat <ariya.hidayat@gmail.com>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "kineticmodel.h"

#include <QTimer>
#include <QTime>

static const int KineticModelDefaultUpdateInterval = 15; // ms

class KineticModelPrivate
{
public:
    QTimer ticker;

    int duration;
    QPointF position;
    qreal heading;
    QPointF velocity;
    qreal velocityHeading;
    QPointF deacceleration;
    qreal deaccelerationHeading;

    QTime timestamp;
    QPointF lastPosition;
    qreal lastHeading;
    bool changingPosition;

    KineticModelPrivate();
};

KineticModelPrivate::KineticModelPrivate()
    : duration(1403)
    , position(0, 0)
    , heading(0)
    , velocity(0, 0)
    , velocityHeading(0)
    , deacceleration(0, 0)
    , deaccelerationHeading(0)
    , lastPosition(0, 0)
    , lastHeading(0)
    , changingPosition(true)
{

}

KineticModel::KineticModel(QObject *parent)
    : QObject(parent)
    , d_ptr(new KineticModelPrivate)
{
    connect(&d_ptr->ticker, SIGNAL(timeout()), SLOT(update()));
    d_ptr->ticker.setInterval(KineticModelDefaultUpdateInterval);
}

KineticModel::~KineticModel()
{

}

bool KineticModel::hasVelocity() const
{
    return !d_ptr->velocity.isNull();
}

int KineticModel::duration() const
{
    return d_ptr->duration;
}

void KineticModel::setDuration(int ms)
{
    d_ptr->duration = ms;
}

QPointF KineticModel::position() const
{
    return d_ptr->position;
}

void KineticModel::setPosition(const QPointF& position)
{
    setPosition( position.x(), position.y() );
}

void KineticModel::setPosition(qreal posX, qreal posY)
{
    d_ptr->position.setX( posX );
    d_ptr->position.setY( posY );

    int elapsed = d_ptr->timestamp.elapsed();

    // too fast gives less accuracy
    if (elapsed < d_ptr->ticker.interval() / 2) {
        return;
    }

    qreal delta = static_cast<qreal>( elapsed ) / 1000.0;

    QPointF lastSpeed = d_ptr->velocity;
    QPointF currentSpeed = ( d_ptr->position - d_ptr->lastPosition ) / delta;
    d_ptr->velocity = 0.2 * lastSpeed + 0.8 * currentSpeed;
    d_ptr->lastPosition = d_ptr->position;

    d_ptr->changingPosition = true;
    d_ptr->timestamp.start();
}

void KineticModel::setHeading(qreal heading)
{
    d_ptr->heading = heading;

    int elapsed = d_ptr->timestamp.elapsed();
    qreal delta = static_cast<qreal>( elapsed ) / 1000.0;

    qreal lastSpeed = d_ptr->velocityHeading;
    qreal currentSpeed = delta ? ( d_ptr->heading - d_ptr->lastHeading ) / delta : 0;
    d_ptr->velocityHeading = 0.5 * lastSpeed + 0.2 * currentSpeed;
    d_ptr->lastHeading = d_ptr->heading;

    d_ptr->changingPosition = false;
    d_ptr->timestamp.start();
}

void KineticModel::jumpToPosition(const QPointF& position)
{
    jumpToPosition( position.x(), position.y() );
}

void KineticModel::jumpToPosition(qreal posX, qreal posY)
{
    d_ptr->position.setX( posX );
    d_ptr->position.setY( posY );
}

int KineticModel::updateInterval() const
{
    return d_ptr->ticker.interval();
}

void KineticModel::setUpdateInterval(int ms)
{
    d_ptr->ticker.setInterval(ms);
}

void KineticModel::stop()
{
    Q_D(KineticModel);

    d->ticker.stop();
    d->timestamp.start();
    d->velocity = QPointF(0, 0);
    d->velocityHeading = 0;
}

void KineticModel::start()
{
    Q_D(KineticModel);

    // prevent kinetic spinning if last mouse move is too long ago
    const int elapsed = d->timestamp.elapsed();
    if ( elapsed > 2 * d->ticker.interval() ) {
        d->ticker.stop();
        emit finished();
        return;
    }

    d->deacceleration = d->velocity * 1000 / ( 1 + d_ptr->duration );
    if (d->deacceleration.x() < 0) {
        d->deacceleration.setX( -d->deacceleration.x() );
    }
    if (d->deacceleration.y() < 0) {
        d->deacceleration.setY( -d->deacceleration.y() );
    }

    d->deaccelerationHeading = qAbs(d->velocityHeading) * 1000 / ( 1 + d_ptr->duration );

    if (!d->ticker.isActive())
        d->ticker.start();
}

void KineticModel::update()
{
    Q_D(KineticModel);

    int elapsed = qMin( d->timestamp.elapsed(), 100 ); // limit to 100msec to reduce catapult effect (bug 294608)
    qreal delta = static_cast<qreal>(elapsed) / 1000.0;

    bool stop = false;
    if (d->changingPosition) {
        d->position += d->velocity * delta;
        QPointF vstep = d->deacceleration * delta;

        if (d->velocity.x() < vstep.x() && d->velocity.x() >= -vstep.x()) {
            d->velocity.setX( 0 );
        } else {
            if (d->velocity.x() > 0)
                d->velocity.setX( d->velocity.x() - vstep.x() );
            else
                d->velocity.setX( d->velocity.x() + vstep.x() );
        }

        if (d->velocity.y() < vstep.y() && d->velocity.y() >= -vstep.y()) {
            d->velocity.setY( 0 );
        } else {
            if (d->velocity.y() > 0)
                d->velocity.setY( d->velocity.y() - vstep.y() );
            else
                d->velocity.setY( d->velocity.y() + vstep.y() );
        }

        stop = d->velocity.isNull();

        emit positionChanged( d->position.x(), d->position.y() );
    } else {
        d->heading += d->velocityHeading * delta;
        qreal vstep = d->deaccelerationHeading * delta; // Always positive.
        if ((d->velocityHeading < vstep && d->velocityHeading >= -vstep) || !vstep) {
            d->velocityHeading = 0;
        } else {
            d->velocityHeading += d->velocityHeading > 0 ? -1 * vstep : vstep;
        }

        stop = !d->velocityHeading;

        emit headingChanged( d->heading );
    }

    if (stop) {
        emit finished();
        d->ticker.stop();
    }

    d->timestamp.start();
}

#include "moc_kineticmodel.cpp"

