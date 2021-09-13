// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef UPLOAD_H
#define UPLOAD_H

#include "region.h"
#include "jobparameters.h"

#include <QObject>
#include <QList>
#include <QFileInfo>
#include <QDomDocument>

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

    explicit Upload(QObject *parent = nullptr);

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
