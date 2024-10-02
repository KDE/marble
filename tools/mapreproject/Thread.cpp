/*
    SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies) <qt-info@nokia.com>
    All rights reserved.

    This file is part of the examples of the Qt Toolkit.

    SPDX-License-Identifier: BSD-3-Clause
*/

#include "Thread.h"

#include <QSignalMapper>

Thread::Thread(QObject *const parent)
    : QThread(parent)
{
    // we need a class that receives signals from other threads and emits a signal in response
    m_shutDownHelper = new QSignalMapper;
    m_shutDownHelper->setMapping(this, 0);
    connect(this, &Thread::started, this, &Thread::setReadyStatus, Qt::DirectConnection);
    connect(this, SIGNAL(aboutToStop()), m_shutDownHelper, SLOT(map()));
}

Thread::~Thread()
{
    delete m_shutDownHelper;
}

// starts thread, moves worker to this thread and blocks
void Thread::launchWorker(QObject *const worker)
{
    m_worker = worker;
    start();
    m_worker->moveToThread(this);
    m_shutDownHelper->moveToThread(this);
    connect(m_shutDownHelper, &QSignalMapper::mappedInt, this, &Thread::stopExecutor, Qt::DirectConnection);
    m_mutex.lock();
    m_waitCondition.wait(&m_mutex);
}

// puts a command to stop processing in the event queue of worker thread
void Thread::stop()
{
    emit aboutToStop();
}

// methods above this line should be called in ui thread context
// methods below this line are private and will be run in  secondary thread context

void Thread::stopExecutor() // secondary thread context
{
    exit();
}

void Thread::setReadyStatus()
{
    m_waitCondition.wakeAll();
}

#include "moc_Thread.cpp"
