/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "Thread.h"

#include <QtCore/QSignalMapper>

Thread::Thread( QObject * const parent )
    : QThread( parent )
{
    // we need a class that receives signals from other threads and emits a signal in response
    m_shutDownHelper = new QSignalMapper;
    m_shutDownHelper->setMapping( this, 0 );
    connect( this, SIGNAL(started()), this, SLOT(setReadyStatus()), Qt::DirectConnection );
    connect( this, SIGNAL(aboutToStop()), m_shutDownHelper, SLOT(map()));
}

Thread::~Thread()
{
    delete m_shutDownHelper;
}

// starts thread, moves worker to this thread and blocks
void Thread::launchWorker( QObject * const worker )
{
    m_worker = worker;
    start();
    m_worker->moveToThread( this );
    m_shutDownHelper->moveToThread( this );
    connect( m_shutDownHelper, SIGNAL(mapped(int)), this, SLOT(stopExecutor()), Qt::DirectConnection );
    m_mutex.lock();
    m_waitCondition.wait( &m_mutex );
}

// puts a command to stop processing in the event queue of worker thread
void Thread::stop()
{
    emit aboutToStop();
}

// methods above this line should be called in ui thread context
// methods below this line are private and will be run in  secondary thread context

void Thread::stopExecutor()  //secondary thread context
{
    exit();
}

void Thread::setReadyStatus()
{
    m_waitCondition.wakeAll();
}
