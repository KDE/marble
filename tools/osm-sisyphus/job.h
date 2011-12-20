//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef JOB_H
#define JOB_H

#include "jobparameters.h"
#include "region.h"

#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QtCore/QFileInfo>

class Job : public QObject, public QRunnable
{
    Q_OBJECT
public:
    enum Status {
        Waiting,
        Downloading,
        Routing,
        Search,
        Packaging,
        Uploading,
        Finished,
        Error
    };

    explicit Job(const Region &region, const JobParameters &parameters, QObject *parent = 0);

    Status status() const;

    QString statusMessage() const;

    Region region() const;

    void setTransport(const QString &transport);

    void setProfile(const QString &profile);

    void setMonavSettings(const QString &filename);

    bool operator==(const Job &other) const;

    virtual void run();

Q_SIGNALS:
    void finished(Job* job);
    
private:
    void changeStatus(Status status, const QString &message);

    bool download();

    bool monav();

    bool search();

    bool package();

    bool upload();

    bool cleanup();

    QFileInfo osmFile();

    QFileInfo monavDir();

    QFileInfo targetFile();

    QFileInfo searchFile();

    Status m_status;

    Region m_region;

    JobParameters m_parameters;

    QString m_statusMessage;

    QString m_transport;

    QString m_profile;

    QString m_monavSettings;
};

#endif // JOB_H
