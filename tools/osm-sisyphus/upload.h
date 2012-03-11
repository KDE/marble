//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef UPLOAD_H
#define UPLOAD_H

#include "region.h"
#include "jobparameters.h"

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QFileInfo>
#include <QtXml/QDomDocument>

class Upload : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool uploadFiles READ uploadFiles WRITE setUploadFiles)

public:
    static Upload& instance();

    void uploadAndDelete(const Region &region, const QFileInfo &file, const QString &transport);

    bool uploadFiles() const;

    void setJobParameters(const JobParameters &parameters);

    void setUploadFiles(bool arg);

private:
    struct Package {
        Region region;
        QFileInfo file;
        QString transport;

        bool operator==(const Package &other) const;
    };

    explicit Upload(QObject *parent = 0);

    void changeStatus( const Package &package, const QString &status, const QString &message );

    void processQueue();

    bool upload(const Package &package);

    void deleteFile(const QFileInfo &file);

    bool adjustNewstuffFile(const Package &package);

    bool uploadNewstuff();

    bool deleteRemoteFile(const QString &filename);

    QString targetDir() const;

    QString releaseDate() const;

    QList<Package> m_queue;
    bool m_uploadFiles;
    QDomDocument m_xml;
    JobParameters m_jobParameters;
};

#endif // UPLOAD_H
