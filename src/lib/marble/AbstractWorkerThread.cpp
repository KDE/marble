//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "AbstractWorkerThread.h"
#include "MarbleDebug.h"

// Qt
#include <QMutex>

namespace Marble
{

const int WAIT_ATTEMPTS = 20;
const int WAIT_TIME = 100;

class AbstractWorkerThreadPrivate
{
 public:
    explicit AbstractWorkerThreadPrivate( AbstractWorkerThread *parent )
            : m_running( false ),
              m_end( false ),
              m_parent( parent )
    {
    }

    ~AbstractWorkerThreadPrivate()
    {
        m_end = true;
        m_parent->wait( 1000 );
    }

    QMutex m_runningMutex;
    bool m_running;
    bool m_end;

    AbstractWorkerThread *m_parent;
};


AbstractWorkerThread::AbstractWorkerThread( QObject *parent )
        : QThread( parent ),
          d( new AbstractWorkerThreadPrivate( this ) )
{
}

AbstractWorkerThread::~AbstractWorkerThread()
{
    delete d;
}

void AbstractWorkerThread::ensureRunning()
{
    QMutexLocker locker( &d->m_runningMutex );
    if ( !d->m_running ) {
        if ( wait( 2 * WAIT_TIME ) ) {
            d->m_running = true;
            start( QThread::IdlePriority );
        }
    }
}

void AbstractWorkerThread::run()
{
    int waitAttempts = WAIT_ATTEMPTS;
    while( !d->m_end ) {
        d->m_runningMutex.lock();
        if ( !workAvailable() ) {
            waitAttempts--;
            if ( !waitAttempts || d->m_end ) {
                d->m_running = false;
                d->m_runningMutex.unlock();
                break;
            }
            else {
                d->m_runningMutex.unlock();
                msleep( WAIT_TIME );
            }
        }
        else {
            d->m_runningMutex.unlock();
            work();

            waitAttempts = WAIT_ATTEMPTS;
        }
    }
}

}

#include "moc_AbstractWorkerThread.cpp"
