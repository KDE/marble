// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_ABSTRACTWORKERTHREAD_H
#define MARBLE_ABSTRACTWORKERTHREAD_H

// Marble
#include "marble_export.h"

// Qt
#include <QThread>

namespace Marble
{

class AbstractWorkerThreadPrivate;

/**
 * The AbstractWorkerThread is a class written for small tasks that have to run
 * multiple times on different data asynchronously.
 * You should be able to use this class for many different tasks, but you'll have to
 * think about Multi-Threading additionally.
 * The AbstractWorkerThread runs the function work() as long as workAvailable()
 * returns true. If there is no work available for a longer time, the thread will
 * switch itself off. As a result you have to call ensureRunning() every time you
 * want something to be worked on. You'll probably want to call this in your
 * addSchedule() function.
 */
class MARBLE_EXPORT AbstractWorkerThread : public QThread
{
    Q_OBJECT

 public:
    explicit AbstractWorkerThread( QObject *parent = nullptr );
    ~AbstractWorkerThread() override;

    void ensureRunning();

 protected:
    virtual bool workAvailable() = 0;
    virtual void work() = 0;

    void run() override;

 private:
    AbstractWorkerThreadPrivate * const d;
};

}

#endif
