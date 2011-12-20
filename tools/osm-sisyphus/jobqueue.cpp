//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "jobqueue.h"
#include "logger.h"

#include <QtCore/QDebug>
#include <QtCore/QThreadPool>

JobQueue::JobQueue(QObject *parent) :
    QObject(parent), m_maxConcurrentJobs(1)
{
    // nothing to do
}

void JobQueue::addJob(Job *newJob)
{
    QList<Job*> const allJobs = m_jobs + m_runningJobs;
    foreach(Job* job, allJobs) {
        if (*job == *newJob) {
            qDebug() << "Ignoring job, still running";
            return;
        }
    }

    connect(newJob, SIGNAL(finished(Job*)), this, SLOT(removeJob(Job*)));

    if (m_runningJobs.size()<m_maxConcurrentJobs) {
        startJob(newJob);
    } else {
        Logger::instance().setStatus(newJob->region().id(), newJob->region().name(), "waiting", "Queued.");
        m_jobs << newJob;
    }
}

void JobQueue::setMaxConcurrentJobs(int size)
{
    m_maxConcurrentJobs = size;
}

void JobQueue::removeJob(Job *job)
{
    m_runningJobs.removeAll(job);

    while (m_runningJobs.size()<m_maxConcurrentJobs && !m_jobs.isEmpty()) {
        Job* newJob = m_jobs.first();
        m_jobs.removeFirst();
        startJob(newJob);
    }
}

void JobQueue::startJob(Job *job)
{
    m_runningJobs << job;
    QThreadPool::globalInstance()->start(job);
}
