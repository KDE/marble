//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "jobmanager.h"

#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtXml/QDomDocument>

JobManager::JobManager(QObject *parent) :
    QObject(parent)
{   
    QDir temp = QDir(QDir::tempPath());

    m_monavSettings = QFileInfo(temp, "monav_settings.ini");
    QFile settingsFile(m_monavSettings.absoluteFilePath());
    settingsFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream settingsStream(&settingsFile);

    settingsStream << "[General]\npackaging=false\ndictionarySize=16384\nblockSize=16384\n";
    settingsStream << "importer=OpenStreetMap Importer\nrouter=Contraction Hierarchies";
    settingsStream << "renderer=Mapnik Renderer\ngpsLookup=GPS Grid\naddressLookup=Unicode Tournament Trie\n\n";
    settingsStream << "[OSM%20Importer]\nlanguages=name\nspeedProfile=:/speed profiles/motorcar.spp\n\n";
    settingsStream << "[ContractionHierarchies]\nblockSize=12\n";
    settingsFile.close();
}

void JobManager::run()
{
    update();
}

void JobManager::setRegionsFile(const QString &filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly);

    QDomDocument xml;
    if ( !xml.setContent( &file ) ) {
        qDebug() << "Cannot parse xml file with regions.";
        return;
    }

    QDomElement root = xml.documentElement();
    QDomNodeList regions = root.elementsByTagName( "region" );
    for ( unsigned int i = 0; i < regions.length(); ++i ) {
        Region region;
        QDomNode node = regions.item( i );
        if (!node.namedItem("continent").isNull()) {
            region.setContinent(node.namedItem("continent").toElement().text());
        }
        if (!node.namedItem("country").isNull()) {
            region.setCountry(node.namedItem("country").toElement().text());
        }
        if (!node.namedItem("name").isNull()) {
            region.setName(node.namedItem("name").toElement().text());
        }
        if (!node.namedItem("id").isNull()) {
            region.setId(node.namedItem("id").toElement().text());
        }
        if (!node.namedItem("path").isNull()) {
            region.setPath(node.namedItem("path").toElement().text());
        }
        if (!node.namedItem("pbf").isNull()) {
            region.setPbfFile(node.namedItem("pbf").toElement().text());
        }
        if (!node.namedItem("transport").isNull()) {
            QStringList input = node.namedItem("transport").toElement().text().split(',', QString::SkipEmptyParts);
            foreach( const QString &value, input ) {
                if (!region.continent().isEmpty() && !region.name().isEmpty()) {
                    PendingJob job;
                    job.m_region = region;
                    job.m_transport = value.trimmed();
                    if (job.m_transport == "Motorcar") {
                        job.m_profile = "motorcar";
                        m_pendingJobs << job;
                    } else if (job.m_transport == "Bicycle") {
                        job.m_profile = "bicycle";
                        m_pendingJobs << job;
                    } else if (job.m_transport == "Pedestrian") {
                        job.m_profile = "foot";
                        m_pendingJobs << job;
                    } else {
                        qDebug() << "Invalid transport type " << job.m_transport
                                 << " in .xml file, ignoring. Valid types are Motorcar, Bicycle and Pedestrian.";
                    }
                }
            }
        }
    }
}

void JobManager::setResumeId(const QString &resumeId)
{
    m_resumeId = resumeId;
}

void JobManager::setJobParameters(const JobParameters &parameters)
{
    m_jobParameters = parameters;
}

void JobManager::update()
{
    bool resume = m_resumeId.isEmpty();
    foreach(const PendingJob &job, m_pendingJobs) {
        resume = resume || job.m_region.id() == m_resumeId;
        if (resume) {
            addJob(job);
        }
    }

    QTimer::singleShot(1000*60*60*24, this, SLOT(update()));
}

void JobManager::addJob(const PendingJob &job)
{
    Job* countryJob = new Job(job.m_region, m_jobParameters);
    /** @todo: Support other transport types */
    countryJob->setTransport(job.m_transport);
    countryJob->setProfile(job.m_profile);
    countryJob->setMonavSettings(m_monavSettings.absoluteFilePath());
    m_queue.addJob(countryJob);
}
