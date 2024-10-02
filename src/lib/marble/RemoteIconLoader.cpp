// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "RemoteIconLoader.h"

// Qt
#include <QByteArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QString>
#include <QUrl>

// Marble
#include "FileStoragePolicy.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleGlobal.h"

namespace Marble
{

class RemoteIconLoaderPrivate
{
public:
    RemoteIconLoaderPrivate()
        : m_storagePolicy(MarbleDirs::localPath() + QLatin1StringView("/cache/icons/"))
        , m_downloadManager(&m_storagePolicy)
    {
    }
    QHash<QUrl, QImage> m_iconCache;
    FileStoragePolicy m_storagePolicy;
    HttpDownloadManager m_downloadManager;

    /**
     * Returns true if the icon for Url(=url) is available in cache
     */
    bool isCached(const QUrl &url) const;

    /**
     * Returns icon for the url passes in argument
     */
    QImage cachedIcon(const QUrl &url) const;

    /**
     * Returns true if icon is locally present on disk
     */
    bool loadFromDiskCache(const QUrl &url);

    /**
     * Starts downloading icon if it isn't present cache and
     * could not be found locally on disk
     */
    void initiateDownload(const QUrl &url);

    /**
     * Returns a name with which downloaded icon will be saved on disk
     */
    static QString cacheFileName(const QUrl &url);
};

bool RemoteIconLoaderPrivate::isCached(const QUrl &url) const
{
    return m_iconCache.contains(url);
}

QImage RemoteIconLoaderPrivate::cachedIcon(const QUrl &url) const
{
    return m_iconCache.value(url);
}

bool RemoteIconLoaderPrivate::loadFromDiskCache(const QUrl &url)
{
    QString path = MarbleDirs::localPath() + QLatin1StringView("/cache/icons/") + cacheFileName(url);
    QImage icon = QFile::exists(path) ? QImage(path) : QImage();
    if (!icon.isNull()) {
        m_iconCache.insert(url, icon);
        return true;
    }
    return false;
}

void RemoteIconLoaderPrivate::initiateDownload(const QUrl &url)
{
    DownloadUsage usage = DownloadBrowse;
    m_downloadManager.setDownloadEnabled(true);
    QString fileName = cacheFileName(url);
    m_downloadManager.addJob(url, fileName, url.toString(), usage);
}

QString RemoteIconLoaderPrivate::cacheFileName(const QUrl &url)
{
    const QString suffix = QFileInfo(url.path()).suffix();
    const QByteArray hash = QCryptographicHash::hash(url.toEncoded(), QCryptographicHash::Md5).toHex();
    const QString fileName = QString::fromLatin1(hash) + QLatin1Char('.') + suffix;
    return fileName;
}

RemoteIconLoader::RemoteIconLoader(QObject *parent)
    : QObject(parent)
    , d(new RemoteIconLoaderPrivate())
{
    connect(&d->m_downloadManager, SIGNAL(downloadComplete(QByteArray, QString)), this, SLOT(storeIcon(QByteArray, QString)));
}

RemoteIconLoader::~RemoteIconLoader()
{
    delete d;
}

QImage RemoteIconLoader::load(const QUrl &url)
{
    /*
     * If image has been downloaded previously then
     * return it from m_iconCache. All the downloaded
     * images in current running marble session are stored
     * in m_iconCache.
     */
    if (d->isCached(url)) {
        return d->cachedIcon(url);
    }

    /*
     * All the downloaded images are saved on disk
     * at location cache/icons/ ( relative location ).
     * If the rquested icon is present at this location then
     * return it.
     */
    else if (d->loadFromDiskCache(url)) {
        return d->cachedIcon(url);
    }

    // Otherwise initiate download
    else {
        d->initiateDownload(url);
    }
    return {};
}

void RemoteIconLoader::storeIcon(const QByteArray &data, const QString &fileName)
{
    QImage icon = QImage::fromData(data);
    d->m_iconCache.insert(QUrl(fileName), icon);
    Q_EMIT iconReady();
}

}

#include "moc_RemoteIconLoader.cpp"
