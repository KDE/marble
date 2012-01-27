//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "job.h"
#include "logger.h"
#include "upload.h"

#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QProcess>

Job::Job(const Region &region, const JobParameters &parameters, QObject *parent) :
    QObject(parent), m_status(Waiting), m_region(region), m_parameters(parameters)
{
    // nothing to do
}

Job::Status Job::status() const
{
    return m_status;
}

QString Job::statusMessage() const
{
    return m_statusMessage;
}

Region Job::region() const
{
    return m_region;
}

void Job::setTransport(const QString &transport)
{
    m_transport = transport;
}

void Job::setProfile(const QString &profile)
{
    m_profile = profile;
}

void Job::setMonavSettings(const QString &filename)
{
    m_monavSettings = filename;
}

bool Job::operator ==(const Job &other) const
{
    return m_region == other.m_region;
}

void Job::run()
{
    if (download() && monav() && search() && package() && upload()) {
        // Nothing to do.
    }

    cleanup();
    emit finished(this);
}

void Job::changeStatus(Job::Status status, const QString &message)
{
    QString statusType;
    switch (status) {
    case Waiting: statusType = "waiting"; break;
    case Downloading: statusType = "downloading"; break;
    case Routing: statusType = "routing"; break;
    case Search: statusType = "search"; break;
    case Packaging: statusType = "packaging"; break;
    case Uploading: statusType = "uploading"; break;
    case Finished: statusType = "finished"; break;
    case Error: statusType = "error"; break;
    }

    Logger::instance().setStatus(m_region.id(), m_region.name(), statusType, message);
    m_statusMessage = message;
    m_status = status;
}

bool Job::download()
{
    changeStatus(Downloading, "Downloading data.");
    qDebug() << "Saving file to " << osmFile().absoluteFilePath();
    if (osmFile().exists()) {
        QDateTime now = QDateTime::currentDateTime();
        if (osmFile().lastModified().daysTo(now) > 7) {
            qDebug() << "Old file is outdated, re-downloading " << osmFile().absoluteFilePath();
            QFile::remove(osmFile().absoluteFilePath());
        } else {
            qDebug() << "Old file is still ok, reusing" << osmFile().absoluteFilePath();
            return true;
        }
    }

    QProcess wget;
    QStringList arguments;
    QString url = m_region.pbfFile();
    arguments << "-O" << osmFile().absoluteFilePath() << url;
    qDebug() << "Downloading " << url;
    wget.start("wget", arguments);
    wget.waitForFinished(1000 * 60 * 60 * 12); // wait up to 12 hours for download to complete
    if (wget.exitStatus() == QProcess::NormalExit && wget.exitCode() == 0) {
        return true;
    } else {
        qDebug() << "Failed to download " << url;
        QFile::remove(osmFile().absoluteFilePath());
        changeStatus(Error, "Error downloading .osm.pbf file: " + wget.readAllStandardError());
        return false;
    }
}

//bool Job::marble()
//{
//    changeStatus(Routing, "Extracting bounding box.");
//    QStringList arguments;
//    arguments << "--name" << m_region.name();
//    arguments << "--version" << "0.2";
//    arguments << "--date" << QDateTime::currentDateTime().toString("yyyy/dd/MM");
//    arguments << "--transport" << m_transport;
//    arguments << "--payload" << targetFile().fileName();
//    arguments << m_parameters.base().absoluteFilePath("poly/" + m_region.polyFile());
//    arguments << monavDir().absoluteFilePath() + "/marble.kml";
//    QProcess poly2kml;
//    poly2kml.start("poly2kml", arguments);
//    poly2kml.waitForFinished(1000 * 60 * 30); // wait up to half an hour for poly2kml to convert the data
//    if (poly2kml.exitStatus() == QProcess::NormalExit && poly2kml.exitCode() == 0) {
//        qDebug() << "Processed kml file for marble";
//        return true;
//    } else {
//        qDebug() << "poly2kml exiting with status " << poly2kml.exitCode();
//        changeStatus(Error, "Error creating marble.kml: " + poly2kml.readAllStandardError());
//        return false;
//    }
//}

bool Job::monav()
{
    QString const status = QString("Generating offline routing map from %1 (%2).").arg(osmFile().fileName()).arg(Region::fileSize(osmFile()));
    changeStatus(Routing, status);
    QStringList arguments;
    arguments << "-s=" + m_monavSettings;
    arguments << "-i=" + osmFile().absoluteFilePath();
    arguments << "-o=" + monavDir().absoluteFilePath();
    arguments << "-pi=OpenStreetMap Importer" << "-pro=Contraction Hierarchies";
    arguments << "-pg=GPS Grid" << "-di";
    arguments << "-dro=" + m_transport;
    arguments << "--profile=" + m_profile;
    arguments << "-dd" /*<< "-dc"*/;
    QProcess monav;
    monav.start("monav-preprocessor", arguments);
    monav.waitForFinished(1000 * 60 * 60 * 6); // wait up to 6 hours for monav to convert the data
    if (monav.exitStatus() == QProcess::NormalExit && monav.exitCode() == 0) {
        qDebug() << "Processed osm file for monav";
    } else {
        qDebug() << "monav exiting with status " << monav.exitCode();
        changeStatus(Error, "Routing map conversion failed: " + monav.readAllStandardError());
        return false;
    }

    QFile pluginsFile(monavDir().absoluteFilePath() + "/plugins.ini");
    pluginsFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream pluginsStream(&pluginsFile);
    pluginsStream << "[General]\nrouter=Contraction Hierarchies\nrenderer=Mapnik Renderer\ngpsLookup=GPS Grid\naddressLookup=Unicode Tournament Trie\n";
    pluginsFile.close();

    QFileInfo subdir = QFileInfo(monavDir().absoluteFilePath() + "/routing_" + m_transport.toLower());
    if (subdir.exists() && subdir.isDir()) {
        QFileInfoList files = QDir(subdir.absoluteFilePath()).entryInfoList(QDir::Files);
        foreach(const QFileInfo &file, files) {
            if (!QFile::rename(file.absoluteFilePath(), monavDir().absoluteFilePath() + "/" + file.fileName())) {
                changeStatus(Error, "Unable to move monav files to target directory.");
                return false;
            }
        }
        QDir("/").rmdir(subdir.absoluteFilePath());
    } else {
        changeStatus(Error, "Unable to find files created by monav");
        return false;
    }

    return true;
}

bool Job::search()
{
    QString const status = QString("Generating offline search database from %1 (%2).").arg(osmFile().fileName()).arg(Region::fileSize(osmFile()));
    changeStatus(Search, status);
    QStringList arguments;
    arguments << "--name" << m_region.name();
    arguments << "--version" << "0.3";
    arguments << "--date" << QDateTime::currentDateTime().toString("MM/dd/yy");
    arguments << "--transport" << m_transport;
    arguments << "--payload" << targetFile().fileName();
    arguments << osmFile().absoluteFilePath();
    arguments << searchFile().absoluteFilePath();
    QFileInfo kmlFile(monavDir().absoluteFilePath() + "/marble.kml");
    arguments << kmlFile.absoluteFilePath();
    QProcess osmAddresses;
    osmAddresses.start("osm-addresses", arguments);
    osmAddresses.waitForFinished(1000 * 60 * 60 * 18); // wait up to 18 hours for osm-addresses to convert the data
    if (osmAddresses.exitStatus() == QProcess::NormalExit && osmAddresses.exitCode() == 0) {
        searchFile().refresh();
        if (!searchFile().exists()) {
            qDebug() << "osm-addresses did not create the .sqlite file";
            changeStatus(Error, "Unknown error when creating the search database");
            return false;
        } else if (searchFile().size() < 8000) {
            qDebug() << "The .sqlite database has a suspiciously small size.";
            changeStatus(Error, "Search database is too small. Too little memory?");
            return false;
        }

        kmlFile.refresh();
        if (!kmlFile.exists()) {
            qDebug() << "File marble.kml has not been generated.";
            changeStatus(Error, "Failed to generate marble.kml. Too little memory?");
            return false;
        }

        return true;
    } else {
        qDebug() << "osm-addresses exiting with status " << osmAddresses.exitCode();
        changeStatus(Error, "Error creating search database: " + osmAddresses.readAllStandardError());
        return false;
    }
}

bool Job::package()
{
    changeStatus(Packaging, "Creating archive.");
    QStringList arguments;
    arguments << "czf" << targetFile().absoluteFilePath() << "earth/monav/" << "earth/placemarks";
    QProcess tar;
    tar.setWorkingDirectory(m_parameters.base().absolutePath() + "/data/" + m_region.id());
    tar.start("tar", arguments);
    tar.waitForFinished(1000 * 60 * 60); // wait up to 1 hour for tar to package things
    if (tar.exitStatus() == QProcess::NormalExit && tar.exitCode() == 0) {
        qDebug() << "Packaged tar file";
        return true;
    } else {
        changeStatus(Error, "Packaging failed: " + tar.readAllStandardError());
        return false;
    }
}

bool Job::upload()
{
    changeStatus(Uploading, "Uploading file");
    if (targetFile().exists()) {
        Upload::instance().uploadAndDelete(m_region, targetFile(), m_transport);
        return true;
    }

    changeStatus(Error, "Target file does not exist.");
    return false;
}

bool Job::cleanup()
{
    if (!m_parameters.cacheData()) {
        QFile::remove(osmFile().absoluteFilePath());
    }

    QFileInfo subdir = QFileInfo(monavDir().absoluteFilePath());
    if (subdir.exists() && subdir.isDir()) {
        QFileInfoList files = QDir(subdir.absoluteFilePath()).entryInfoList(QDir::Files);
        foreach(const QFileInfo &file, files) {
            QFile::remove(file.absoluteFilePath());
        }
    }

    QFile::remove(searchFile().absoluteFilePath());
    return true;
}

QFileInfo Job::osmFile()
{
    m_parameters.base().mkdir("download");
    QFileInfo result(m_parameters.base(), QString("download/") + m_region.id() + ".osm.pbf");
    return result;
}

QFileInfo Job::monavDir()
{
    QString const subdir = "data/" + m_region.id() + "/earth/monav/" + m_transport.toLower() + "/" + m_region.path();
    m_parameters.base().mkpath(subdir);
    QFileInfo result(m_parameters.base(), subdir);
    return result;
}

QFileInfo Job::targetFile()
{
    m_parameters.base().mkdir("finished");
    QFileInfo result(m_parameters.base(), "finished/" + m_region.id() + "_" + m_transport.toLower() + ".tar.gz");
    return result;
}

QFileInfo Job::searchFile()
{
    QString const subdir = "data/" + m_region.id() + "/earth/placemarks/" + QFileInfo(m_region.path()).path();
    m_parameters.base().mkpath(subdir);
    QFileInfo result(m_parameters.base(), subdir + "/" + m_region.id() + ".sqlite");
    return result;
}
