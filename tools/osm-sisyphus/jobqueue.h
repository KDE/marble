// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include "job.h"

#include <QObject>
#include <QList>

class JobQueue : public QObject
{
    Q_OBJECT
public:
    explicit JobQueue(QObject *parent = nullptr);

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
