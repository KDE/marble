//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Own
#include "FileStorageWatcher.h"

// Qt
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QTimer>

// Marble
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"

using namespace Marble;

// Only remove 20 files without checking
// changed cacheLimits and changed themes etc.
static const int maxFilesDelete = 20;
static const int softLimitPercent = 5;


// Methods of FileStorageWatcherThread
FileStorageWatcherThread::FileStorageWatcherThread( const QString &dataDirectory, QObject *parent )
    : QObject( parent ),
      m_dataDirectory( dataDirectory ),
      m_deleting( false ),
      m_willQuit( false )
{
    // For now setting cache limit to 0. This won't delete anything
    setCacheLimit( 0 );
    
    connect( this, SIGNAL(variableChanged()),
	     this, SLOT(ensureCacheSize()),
	     Qt::QueuedConnection );
    emit variableChanged();
}

FileStorageWatcherThread::~FileStorageWatcherThread()
{
}

quint64 FileStorageWatcherThread::cacheLimit()
{
    return m_cacheLimit;
}

void FileStorageWatcherThread::setCacheLimit( quint64 bytes )
{
    m_limitMutex.lock();
    m_cacheLimit = bytes;
    m_cacheSoftLimit = bytes / 100 * ( 100 - softLimitPercent );
    m_limitMutex.unlock();
    emit variableChanged();
}

void FileStorageWatcherThread::addToCurrentSize( qint64 bytes )
{
//     mDebug() << "Current cache size changed by " << bytes;
    qint64 changedSize = bytes + m_currentCacheSize;
    if( changedSize >= 0 )
	m_currentCacheSize = changedSize;
    else
	m_currentCacheSize = 0;
    emit variableChanged();
}

void FileStorageWatcherThread::resetCurrentSize()
{
    m_currentCacheSize = 0;
    emit variableChanged();
}

void FileStorageWatcherThread::prepareQuit()
{
    m_willQuit = true;
}

void FileStorageWatcherThread::getCurrentCacheSize()
{
    mDebug() << "FileStorageWatcher: Creating cache size";
    quint64 dataSize = 0;
    const QString basePath = m_dataDirectory + QLatin1String("/maps");
    QDirIterator it( basePath,
                     QDir::Files | QDir::Writable,
                     QDirIterator::Subdirectories );
    
    const int basePathDepth = basePath.split(QLatin1Char('/')).size();
    while( it.hasNext() && !m_willQuit ) {
        it.next();
        QFileInfo file = it.fileInfo();
        // We try to be very careful and just delete images
        // FIXME, when vectortiling I suppose also vector tiles will have
        // to be deleted
        QString suffix = file.suffix().toLower();
        const QStringList path = file.path().split(QLatin1Char('/'));

        // planet/theme/tilelevel should be deeper than 4
        if ( ( path.size() > basePathDepth + 3 ) &&
             ( path[basePathDepth + 2].toInt() >= maxBaseTileLevel ) &&
             ((suffix == QLatin1String("jpg") ||
               suffix == QLatin1String("png") ||
               suffix == QLatin1String("gif") ||
               suffix == QLatin1String("svg")))) {
            dataSize += file.size();
            m_filesCache.insert(file.lastModified(), file.absoluteFilePath());
        }
    }
    m_currentCacheSize = dataSize;
}

void FileStorageWatcherThread::ensureCacheSize()
{
//     mDebug() << "Size of tile cache: " << m_currentCacheSize;
    // We start deleting files if m_currentCacheSize is larger than
    // the hard cache limit. Then we delete files until our cache size
    // is smaller than the cache limit.
    // m_cacheLimit = 0 means no limit.
    if(    (    ( m_currentCacheSize > m_cacheLimit )
	     || ( m_deleting && ( m_currentCacheSize > m_cacheSoftLimit ) ) )
	&& ( m_cacheLimit != 0 )
	&& ( m_cacheSoftLimit != 0 )
    && !m_willQuit ) {

        mDebug() << "Deleting extra cached tiles";
        // The counter for deleted files
        m_filesDeleted = 0;
        // We have not reached our soft limit, yet.
        m_deleting = true;

        QMultiMap<QDateTime, QString>::iterator it= m_filesCache.begin();
        while ( it != m_filesCache.end() &&
                keepDeleting() ) {
            QString filePath = it.value();
            QFileInfo info( filePath );

            m_filesDeleted++;
            m_currentCacheSize -= info.size();
            it = m_filesCache.erase(it);
            QFile::remove( filePath );
        }

        // We have deleted enough files.
        // Perhaps there are changes.
        if( m_filesDeleted > maxFilesDelete ) {
            QTimer::singleShot( 1000, this, SLOT(ensureCacheSize()) );
            return;
        }
        else {
            // We haven't stopped because of too many files
            m_deleting = false;
        }

        if( m_currentCacheSize > m_cacheSoftLimit ) {
            mDebug() << "FileStorageWatcher: Could not set cache size.";
            // Set the cache limit to a higher value, so we won't start
            // trying to delete something next time.  Softlimit is now exactly
            // on the current cache size.
            setCacheLimit( m_currentCacheSize / ( 100 - softLimitPercent ) * 100 );
        }
    }
}

bool FileStorageWatcherThread::keepDeleting() const
{
    return ( ( m_currentCacheSize > m_cacheSoftLimit ) &&
	     ( m_filesDeleted <= maxFilesDelete ) &&
              !m_willQuit );
}
// End of methods of our Thread


// Beginning of Methods of the main class
FileStorageWatcher::FileStorageWatcher( const QString &dataDirectory, QObject * parent )
    : QThread( parent ),
      m_dataDirectory( dataDirectory )
{
    if ( m_dataDirectory.isEmpty() )
        m_dataDirectory = MarbleDirs::localPath() + QLatin1String("/cache/");
 
    if ( ! QDir( m_dataDirectory ).exists() ) 
        QDir::root().mkpath( m_dataDirectory );
    
    m_started = false;
    m_limitMutex = new QMutex();
    
    m_thread = 0;
    m_quitting = false;
}

FileStorageWatcher::~FileStorageWatcher()
{
    mDebug() << "Deleting FileStorageWatcher";
    
    // Making sure that Thread is stopped.
    m_quitting = true;
    
    if( m_thread )
	m_thread->prepareQuit();
    quit();
    if( !wait( 5000 ) ) {
	mDebug() << "Failed to stop FileStorageWatcher-Thread, terminating!";
	terminate();
    }
    
    delete m_thread;
    
    delete m_limitMutex;
}

void FileStorageWatcher::setCacheLimit( quint64 bytes )
{
    QMutexLocker locker( m_limitMutex );
    if( m_started )
	// This is done directly to ensure that a running ensureCacheSize()
	// recognizes the new size.
	m_thread->setCacheLimit( bytes );
    // Save the limit, thread has to be initialized with the right one.
    m_limit = bytes;
}

quint64 FileStorageWatcher::cacheLimit()
{
    if( m_started )
	return m_thread->cacheLimit();
    else
	return m_limit;
}

void FileStorageWatcher::addToCurrentSize( qint64 bytes )
{
    emit sizeChanged( bytes );
}

void FileStorageWatcher::resetCurrentSize()
{
    emit cleared();
}

void FileStorageWatcher::run()
{
    m_thread = new FileStorageWatcherThread( m_dataDirectory );
    if( !m_quitting ) {
        m_limitMutex->lock();
        m_thread->setCacheLimit( m_limit );
        m_started = true;
        m_limitMutex->unlock();

        m_thread->getCurrentCacheSize();

        connect( this, SIGNAL(sizeChanged(qint64)),
                 m_thread, SLOT(addToCurrentSize(qint64)) );
        connect( this, SIGNAL(cleared()),
                 m_thread, SLOT(resetCurrentSize()) );

        // Make sure that we don't want to stop process.
        // The thread wouldn't exit from event loop.
        if( !m_quitting )
            exec();

        m_started = false;
    }
    delete m_thread;
    m_thread = 0;
}
// End of all methods

#include "moc_FileStorageWatcher.cpp"
