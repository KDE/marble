// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef JOBMANAGER_H
#define JOBMANAGER_H

#include "jobqueue.h"
#include "region.h"

#include <QObject>
#include <QVector>

struct PendingJob {
    Region m_region;
    QString m_transport;
    QString m_profile;
};

class JobManager : public QObject
{
    Q_OBJECT
public:
    explicit JobManager(QObject *parent = nullptr);

    void run();

    void setRegionsFile(const QString &filename);

    void setResumeId(const QString &resumeId);

    void setJobParameters(const JobParameters &parameters);

private Q_SLOTS:
    void update();

private:
    void addJob(const PendingJob &region);

    JobQueue m_queue;

    JobParameters m_jobParameters;

    QFileInfo m_monavSettings;

    QVector<PendingJob> m_pendingJobs;

    QString m_resumeId;
};

#endif // JOBMANAGER_H
