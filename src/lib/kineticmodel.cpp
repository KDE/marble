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

#include <QtCore/QTimer>
#include <QtCore/QDateTime>

static const int KineticModelDefaultUpdateInterval = 15; // ms

class KineticModelPrivate
{
public:
    QTimer ticker;

    int duration;
    QPointF position;
    QPointF velocity;
    QPointF deacceleration;

    QTime timestamp;
    QPointF lastPosition;

    KineticModelPrivate();
};

KineticModelPrivate::KineticModelPrivate()
    : duration(1403)
    , position(0, 0)
    , velocity(0, 0)
    , deacceleration(0, 0)
    , lastPosition(0, 0)
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

void KineticModel::setPosition(QPointF position)
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

    d_ptr->timestamp.start();
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

    if (!d->ticker.isActive())
        d->ticker.start();
}

void KineticModel::update()
{
    Q_D(KineticModel);

    int elapsed = qMin( d->timestamp.elapsed(), 100 ); // limit to 100msec to reduce catapult effect (bug 294608)
    qreal delta = static_cast<qreal>(elapsed) / 1000.0;

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

    emit positionChanged( d->position.x(), d->position.y() );

    if (d->velocity.isNull()) {
        emit finished();
        d->ticker.stop();
    }

    d->timestamp.start();
}

#include "kineticmodel.moc"

