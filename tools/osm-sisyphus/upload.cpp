#include "upload.h"

#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QDateTime>

Upload::Upload(QObject *parent) :
    QObject(parent)
{
    // nothing to do
}

void Upload::processQueue()
{
    if (m_queue.isEmpty()) {
        return;
    }

    Package const package = m_queue.takeFirst();
    upload(package);
    deleteFile(package.file);
    processQueue();
}

void Upload::upload(const Package &package)
{
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
    }

    /** @todo: Adjust monav-maps.xml*/
}

void Upload::deleteFile(const QFileInfo &file)
{
    QFile::remove(file.absoluteFilePath());
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
