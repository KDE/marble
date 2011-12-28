#include "upload.h"

#include "logger.h"

#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QDateTime>

Upload::Upload(QObject *parent) :
    QObject(parent), m_cacheDownloads(false), m_uploadFiles(true)
{
    // nothing to do
}

void Upload::processQueue()
{
    if (m_queue.isEmpty()) {
        return;
    }

    Package const package = m_queue.takeFirst();

    if (upload(package)) {
        QString const message = QString("File %1 (%2) successfully created and uploaded").arg(package.file.fileName()).arg(Region::fileSize(package.file));
        Logger::instance().setStatus(package.region.id(), package.region.name(), "finished", message);
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
    QString const auth = "marble@files.kde.org";
    arguments << auth;
    arguments << "mkdir" << "-p";
    QString targetDir = "/home/marble/web/monav/%1-w%2";
    targetDir = targetDir.arg(QDateTime::currentDateTime().date().year());
    targetDir = targetDir.arg(QDateTime::currentDateTime().date().weekNumber());
    arguments << targetDir;
    ssh.start("ssh", arguments);
    ssh.waitForFinished(1000 * 60 * 10); // wait up to 10 minutes for mkdir to complete
    if (ssh.exitStatus() != QProcess::NormalExit || ssh.exitCode() != 0) {
        qDebug() << "Failed to create remote directory " << targetDir;
        Logger::instance().setStatus(package.region.id(), package.region.name(), "error", "Failed to create remote directory: " + ssh.readAllStandardError());
    }

    QProcess scp;
    arguments.clear();
    arguments << package.file.absoluteFilePath();
    QString target = targetDir + "/" + package.file.fileName();
    arguments << auth + ":" + target;
    scp.start("scp", arguments);
    scp.waitForFinished(1000 * 60 * 60 * 12); // wait up to 12 hours for upload to complete
    if (scp.exitStatus() != QProcess::NormalExit || scp.exitCode() != 0) {
        qDebug() << "Failed to upload " << target;
        Logger::instance().setStatus(package.region.id(), package.region.name(), "error", "Failed to upload file: " + scp.readAllStandardError());
    }

    /** @todo: Adjust monav-maps.xml*/
    return true;
}

void Upload::deleteFile(const QFileInfo &file)
{
    if (!m_cacheDownloads) {
        QFile::remove(file.absoluteFilePath());
    }
}

void Upload::uploadAndDelete(const Region &region, const QFileInfo &file)
{
    Package package;
    package.region = region;
    package.file = file;

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

bool Upload::cacheDownloads() const
{
    return m_cacheDownloads;
}

bool Upload::uploadFiles() const
{
    return m_uploadFiles;
}

void Upload::setCacheDownloads(bool arg)
{
    m_cacheDownloads = arg;
}

void Upload::setUploadFiles(bool arg)
{
    m_uploadFiles = arg;
}
