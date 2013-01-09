//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "upload.h"

#include "logger.h"

#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QUrl>
#include <QtXml/QDomDocument>

Upload::Upload(QObject *parent) :
    QObject(parent), m_uploadFiles(true)
{
    // nothing to do
}

void Upload::changeStatus(const Package &package, const QString &status, const QString &message)
{
    Logger::instance().setStatus( package.region.id() + '_' + package.transport,
                                  package.region.name() + QLatin1String( " (" ) + package.transport + ')', status, message);
}

void Upload::processQueue()
{
    if (m_queue.isEmpty()) {
        return;
    }

    Package const package = m_queue.takeFirst();

    if (upload(package)) {
        QString const message = QString("File %1 (%2) successfully created and uploaded").arg(package.file.fileName()).arg(Region::fileSize(package.file));
        changeStatus( package, "finished", message);
    }
    deleteFile(package.file);
    processQueue();
}

bool Upload::upload(const Package &package)
{
    if (!m_uploadFiles) {
        return true;
    }

    QProcess ssh;
    QStringList arguments;
    QString const auth = "marble@filesmaster.kde.org";
    arguments << auth;
    arguments << "mkdir" << "-p";
    QString remoteDir = QString("/home/marble/web/monav/") + targetDir();
    arguments << remoteDir;
    ssh.start("ssh", arguments);
    ssh.waitForFinished(1000 * 60 * 10); // wait up to 10 minutes for mkdir to complete
    if (ssh.exitStatus() != QProcess::NormalExit || ssh.exitCode() != 0) {
        qDebug() << "Failed to create remote directory " << remoteDir;
        changeStatus( package, "error", "Failed to create remote directory: " + ssh.readAllStandardError());
        return false;
    }

    QProcess scp;
    arguments.clear();
    arguments << package.file.absoluteFilePath();
    QString target = remoteDir + '/' + package.file.fileName();
    arguments << auth + ':' + target;
    scp.start("scp", arguments);
    scp.waitForFinished(1000 * 60 * 60 * 12); // wait up to 12 hours for upload to complete
    if (scp.exitStatus() != QProcess::NormalExit || scp.exitCode() != 0) {
        qDebug() << "Failed to upload " << target;
        changeStatus( package, "error", "Failed to upload file: " + scp.readAllStandardError());
        return false;
    }

    return adjustNewstuffFile(package);
}

void Upload::deleteFile(const QFileInfo &file)
{
    if (!m_jobParameters.cacheData()) {
        QFile::remove(file.absoluteFilePath());
    }
}

bool Upload::adjustNewstuffFile(const Package &package)
{
    if (m_xml.isNull()) {
        QTemporaryFile tempFile(QDir::tempPath() + "/monav-maps-XXXXXX.xml");
        tempFile.setAutoRemove(false);
        tempFile.open();
        QString monavFilename = tempFile.fileName();
        tempFile.close();
        QProcess wget;
        QStringList const arguments = QStringList() << "http://filesmaster.kde.org/marble/newstuff/maps-monav.xml" << "-O" << monavFilename;
        wget.start("wget", arguments);
        wget.waitForFinished(1000 * 60 * 60 * 12); // wait up to 12 hours for download to complete
        if (wget.exitStatus() != QProcess::NormalExit || wget.exitCode() != 0) {
            qDebug() << "Failed to download newstuff file from filesmaster.kde.org";
            changeStatus( package, "error", "Failed to sync newstuff file: " + wget.readAllStandardError());
            return false;
        }

        QFile file(monavFilename);
        if (!file.open(QFile::ReadOnly)) {
            qDebug() << "Failed to open newstuff file" << monavFilename;
            changeStatus( package, "error", "Failed to open newstuff file.");
            return false;
        }

        if ( !m_xml.setContent( &file ) ) {
            qDebug() << "Cannot parse newstuff xml file.";
            changeStatus( package, "error", "Failed to parse newstuff .xml file.");
            return false;
        }

        QFile::remove(monavFilename);
    }

    QDomElement root = m_xml.documentElement();
    QDomNodeList regions = root.elementsByTagName( "stuff" );
    for ( unsigned int i = 0; i < regions.length(); ++i ) {
        QDomNode node = regions.item( i );
        if (!node.namedItem("payload").isNull()) {
            QUrl url = node.namedItem("payload").toElement().text();
            QFileInfo fileInfo(url.path());
            if (fileInfo.fileName() == package.file.fileName()) {
                QString removeFile;
                QDomNode dateNode = node.namedItem("releasedate");
                if (!dateNode.isNull()) {
                    dateNode.removeChild(dateNode.firstChild());
                    dateNode.appendChild(m_xml.createTextNode(releaseDate()));
                }
                QDomNode versionNode = node.namedItem("version");
                if (!versionNode.isNull()) {
                    double version = versionNode.toElement().text().toDouble();
                    versionNode.removeChild(versionNode.firstChild());
                    versionNode.appendChild(m_xml.createTextNode(QString::number(version+0.1, 'f', 1)));
                }
                QDomNode payloadNode = node.namedItem("payload");
                payloadNode.removeChild(payloadNode.firstChild());
                if (fileInfo.dir().dirName() != targetDir()) {
                    removeFile = QString("/home/marble/web/monav/%1/%2").arg(fileInfo.dir().dirName()).arg(package.file.fileName());
                    qDebug() << "Going to remove the old file " << removeFile;
                }
                QString payload = "http://files.kde.org/marble/monav/%1/%2";
                payload = payload.arg(targetDir()).arg(package.file.fileName());
                payloadNode.appendChild(m_xml.createTextNode(payload));
                return removeFile.isEmpty() ? uploadNewstuff() : (uploadNewstuff() && deleteRemoteFile(removeFile));
            }
        }
    }

    QDomNode stuff = root.appendChild(m_xml.createElement("stuff"));
    stuff.toElement().setAttribute("category", "marble/routing/monav");
    QDomNode nameNode = stuff.appendChild(m_xml.createElement("name"));
    nameNode.toElement().setAttribute("lang", "en");
    QString name = "%1 / %2 (%3)";
    if (package.region.country().isEmpty()) {
        name = name.arg(package.region.continent()).arg(package.region.name());
        name = name.arg(package.transport);
    } else {
        name = "%1 / %2 / %3 (%4)";
        name = name.arg(package.region.continent()).arg(package.region.country());
        name = name.arg(package.region.name()).arg(package.transport);
    }
    nameNode.appendChild(m_xml.createTextNode(name));

    QDomNode authorNode = stuff.appendChild(m_xml.createElement("author"));
    authorNode.appendChild(m_xml.createTextNode("Automatically created from map data assembled by the OpenStreetMap community"));

    QDomNode licenseNode = stuff.appendChild(m_xml.createElement("license"));
    licenseNode.appendChild(m_xml.createTextNode("Creative Commons by-SA 2.0"));

    QDomNode summaryNode = stuff.appendChild(m_xml.createElement("summary"));
    QString summary = "Requires KDE >= 4.6: Offline Routing in %1, %2";
    summary = summary.arg(package.region.name()).arg(package.region.continent());
    summaryNode.appendChild(m_xml.createTextNode(summary));

    QDomNode versionNode = stuff.appendChild(m_xml.createElement("version"));
    versionNode.appendChild(m_xml.createTextNode("0.1"));

    QDomNode dateNode = stuff.appendChild(m_xml.createElement("releasedate"));
    dateNode.appendChild(m_xml.createTextNode(releaseDate()));

    QDomNode previewNode = stuff.appendChild(m_xml.createElement("preview"));
    QString preview = "http://files.kde.org/marble/monav/previews/%1-preview.png";
    preview = preview.arg(package.region.id());
    previewNode.appendChild(m_xml.createTextNode(preview));

    QDomNode payloadNode = stuff.appendChild(m_xml.createElement("payload"));
    payloadNode.toElement().setAttribute("lang", "en");
    QString payload = "http://files.kde.org/marble/monav/%1/%2";
    payload = payload.arg(targetDir()).arg(package.file.fileName());
    payloadNode.appendChild(m_xml.createTextNode(payload));

    return uploadNewstuff();
}

bool Upload::uploadNewstuff()
{
    QTemporaryFile outFile(QDir::tempPath() + "/monav-maps-out-XXXXXX.xml");
    outFile.open();
    QTextStream outStream(&outFile);
    outStream << m_xml.toString(2);
    outStream.flush();

    QProcess scp;
    QStringList arguments;
    arguments << outFile.fileName();
    arguments << "marble@filesmaster.kde.org:/home/marble/web/newstuff/maps-monav.xml";
    scp.start("scp", arguments);
    scp.waitForFinished(1000 * 60 * 60 * 12); // wait up to 12 hours for upload to complete
    if (scp.exitStatus() != QProcess::NormalExit || scp.exitCode() != 0) {
        qDebug() << "Failed to upload " << outFile.fileName() << ": " << scp.readAllStandardError();
        return false;
    }

    return true;
}

bool Upload::deleteRemoteFile(const QString &filename)
{
    if (filename.isEmpty()) {
        return true;
    }

    if (!filename.startsWith(QLatin1String( "/home/marble/" ))) {
        return false;
    }

    QProcess ssh;
    QStringList arguments;
    arguments << "marble@filesmaster.kde.org" << "rm" << filename;
    ssh.start("ssh", arguments);
    ssh.waitForFinished(1000 * 60 * 10); // wait up to 10 minutes for rm to complete
    if (ssh.exitStatus() != QProcess::NormalExit || ssh.exitCode() != 0) {
        qDebug() << "Failed to delete remote file " << filename;
        return false;
    }

    return true;
}

void Upload::uploadAndDelete(const Region &region, const QFileInfo &file, const QString &transport)
{
    Package package;
    package.region = region;
    package.file = file;
    package.transport = transport;

    m_queue.removeAll(package);
    m_queue << package;
    processQueue();
}

bool Upload::Package::operator ==(const Upload::Package &other) const
{
    return region == other.region;
}

Upload &Upload::instance()
{
    static Upload m_instance;
    return m_instance;
}

bool Upload::uploadFiles() const
{
    return m_uploadFiles;
}

void Upload::setJobParameters(const JobParameters &parameters)
{
    m_jobParameters = parameters;
}

void Upload::setUploadFiles(bool arg)
{
    m_uploadFiles = arg;
}

QString Upload::targetDir() const
{
    QString targetDir = "%1-w%2";
    targetDir = targetDir.arg(QDateTime::currentDateTime().date().year());
    targetDir = targetDir.arg(QDateTime::currentDateTime().date().weekNumber());
    return targetDir;
}

QString Upload::releaseDate() const
{
    return QDateTime::currentDateTime().toString("MM/dd/yy");
}
