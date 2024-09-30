// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "jobmanager.h"
#include "logger.h"
#include "upload.h"

#include <QCoreApplication>
#include <QDebug>

void usage(const QString &app)
{
    qDebug() << "Usage: " << app << "[options] regions.xml log.sqlite [resume-id]";
    qDebug() << "\nOptions:";
    qDebug() << "\t-h, --help................. Show this help";
    qDebug() << "\t-cd, --cache-data.......... Do not delete downloaded .osm.pbf and converted .tar.gz files after a successful conversion and upload";
    qDebug() << "\t-nu, --no-uploads.......... Do not upload converted files to files.kde.org";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList arguments;
    bool cacheData(false);
    bool uploadFiles(true);
    for (int i = 1; i < argc; ++i) {
        QString const arg = argv[i];
        if (arg == QLatin1StringView("-h") || arg == QLatin1StringView("--help")) {
            usage(argv[0]);
            return 0;
        } else if (arg == QLatin1StringView("-cd") || arg == QLatin1StringView("--cache-data")) {
            cacheData = true;
        } else if (arg == QLatin1StringView("-nu") || arg == QLatin1StringView("--no-uploads")) {
            uploadFiles = false;
        } else {
            arguments << arg;
        }
    }

    if (arguments.size() < 2) {
        usage(argv[0]);
        return 1;
    }

    Logger::instance().setFilename(arguments.at(1));

    QFileInfo tempDir = QFileInfo(QDir::tempPath(), "osm-sisyphus");
    JobParameters parameters;
    parameters.setBase(QDir(tempDir.absoluteFilePath()));
    parameters.setCacheData(cacheData);

    Upload::instance().setJobParameters(parameters);
    Upload::instance().setUploadFiles(uploadFiles);

    JobManager manager;
    manager.setRegionsFile(arguments.at(0));
    manager.setJobParameters(parameters);
    if (arguments.size() == 3) {
        manager.setResumeId(arguments.at(2));
    }
    manager.run();

    return app.exec();
}
