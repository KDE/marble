//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include "job.h"

#include <QtCore/QObject>
#include <QtCore/QList>

class JobQueue : public QObject
{
    Q_OBJECT
public:
    explicit JobQueue(QObject *parent = 0);

    void addJob(Job* job);

    void setMaxConcurrentJobs(int size);

private Q_SLOTS:
    void removeJob(Job* job);

private:
    void startJob(Job *job);

    QList<Job*> m_jobs;

    QList<Job*> m_runningJobs;

    int m_maxConcurrentJobs;
};

#endif // JOBQUEUE_H
