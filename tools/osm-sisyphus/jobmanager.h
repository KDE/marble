//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef JOBMANAGER_H
#define JOBMANAGER_H

#include "jobqueue.h"
#include "region.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVector>

class JobManager : public QObject
{
    Q_OBJECT
public:
    explicit JobManager(QObject *parent = 0);
    
    void run();

    void setRegionsFile(const QString &filename);

    void setResumeId(const QString &resumeId);

private Q_SLOTS:
    void update();

private:
    void addJob(const Region &region);

    JobQueue m_queue;

    JobParameters m_parameters;

    QFileInfo m_monavSettings;

    QVector<Region> m_regions;

    QString m_resumeId;
};

#endif // JOBMANAGER_H
