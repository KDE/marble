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
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

// Marble
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"

using namespace Marble;

// Only remove 20 files without checking
// changed cacheLimits and changed themes etc.
static const int maxFilesDelete = 20;
// Delete only files that are older than 120 Seconds
static const int deleteOnlyFilesOlderThan = 120;
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
    
    connect( this, SIGNAL( variableChanged() ),
	     this, SLOT( ensureCacheSize() ),
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

void FileStorageWatcherThread::updateTheme( const QString &mapTheme )
{
    mDebug() << "Theme changed to " << mapTheme;
    m_themeMutex.lock();
    m_mapThemeId = mapTheme;
    m_themeMutex.unlock();
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
    QDirIterator it( m_dataDirectory, QDir::Files, QDirIterator::Subdirectories );
    
    while( it.hasNext() && !m_willQuit )
    {
	it.next();
	QFileInfo file = it.fileInfo();
	dataSize += file.size();
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
	&& !( m_mapThemeId.isEmpty() )
	&& !m_willQuit )
    {
	// The counter for deleted files
	m_filesDeleted = 0;
	// We have not reached our soft limit, yet.
	m_deleting = true;
	
	// Make sure that we are in the right directory
	if ( m_dataDirectory.isEmpty() ||
	    !m_dataDirectory.endsWith(QLatin1String( "data" )) )
	{
	    mDebug()
	     << "Error: Refusing to erase files under"
	     << "unknown conditions for safety reasons!";
	    return;
	}
	
	// Which planet do we show now.
	// We have to delete files for this planet at last
	QStringList currentList = m_mapThemeId.split( '/' );
	QString shownPlanet( currentList.first() );
	currentList.removeFirst();
	// The QString to save the path of the directory to the current planet
	QString lastPlanet;
	
	QString cachedMapsDirectory = m_dataDirectory + "/maps";

	// Iterator over planet directories
	QDirIterator it( cachedMapsDirectory,
			 QDir::NoDotAndDotDot | QDir::Dirs );

	while ( it.hasNext() &&
	        keepDeleting() ) {
	    it.next();
	    
	    // Our current planet directory
	    QString planetDirectory = it.filePath();
	    QFileInfo fileInfo = it.fileInfo();
	    
	    // We have found the currently shown planet.
	    // Please delete here at last.
	    if( fileInfo.fileName() == shownPlanet ) {
		lastPlanet = planetDirectory;
		continue;
	    }
	    
	    ensureSizePerPlanet( planetDirectory );
	}
	
	if( keepDeleting() ) {
	    ensureSizePerPlanet( lastPlanet, currentList.first() );
	}
	
	// We have deleted enough files. 
	// Perhaps there are changes.
	if( m_filesDeleted > maxFilesDelete ) {
	    QTimer::singleShot( 100, this, SLOT( ensureCacheSize() ) );
	    return;
	} 
	else {
	    // We haven't stopped because of to many files
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

void FileStorageWatcherThread::ensureSizePerPlanet( const QString &planetDirectory,
                                                    const QString &currentTheme )
{
    mDebug() << "Deleting from folder: " << planetDirectory;
    
    // lastTheme will store the path to our currentTheme
    QString lastTheme;
    
    QDirIterator itPlanet( planetDirectory,
	                   QDir::NoDotAndDotDot | QDir::Dirs );
    while( itPlanet.hasNext() &&
	   keepDeleting() ) {
	itPlanet.next();
	QString themeDirectory = itPlanet.filePath();
	QFileInfo fileInfo = itPlanet.fileInfo();
	
	// We have found the currently shown theme.
	// Please delete here at last.
	if( !currentTheme.isEmpty() && fileInfo.fileName() == currentTheme ) {
	    mDebug() << "FileStorageWatcher: Skipping " << themeDirectory
	             << " for now";
	    lastTheme = themeDirectory;
	    continue;
	}
	
	ensureSizePerTheme( themeDirectory );
    }
    
    if( keepDeleting() ) {
	mDebug() << "Removing files of: "
	         << lastTheme;
	ensureSizePerTheme( lastTheme );
    }
}

static bool greaterThanByNumber( const QString &s1, const QString &s2)
{
    return s1.toInt() > s2.toInt();
}

void FileStorageWatcherThread::ensureSizePerTheme( const QString &themeDirectory )
{
    mDebug() << "Deleting from folder: " << themeDirectory;

    // Delete from folders with high numbers first
    QStringList folders =
	QDir( themeDirectory ).entryList(   QDir::Dirs
					  | QDir::NoDotAndDotDot );
    qSort( folders.begin(), folders.end(), greaterThanByNumber );
    
    QStringListIterator itTheme( folders );
    while ( itTheme.hasNext() &&
	    keepDeleting() ) {
	QString subDirectory = itTheme.next();

	// Do not delete base tiles
	if ( subDirectory.toInt() <= maxBaseTileLevel ) {
	    continue;
	}
	
	QString tileDirectory = themeDirectory + '/' + subDirectory;
	
	QDirIterator itTile( tileDirectory,
			     QDir::Files | QDir::NoSymLinks,
			     QDirIterator::Subdirectories );
	while ( itTile.hasNext() &&
	        keepDeleting() ) {
	    itTile.next();
	    QString filePath = itTile.filePath();
	    QString lowerCase = filePath.toLower();
	
	    QFileInfo info( filePath );
	
	    // We try to be very careful and just delete images
	    // Do not delete files younger than two minutes.
        // FIXME, when vectortiling I suppose also vector tiles will have
        // to be deleted
	    if (   (    lowerCase.endsWith( QLatin1String( ".jpg" ) ) 
	             || lowerCase.endsWith( QLatin1String( ".png" ) )
	             || lowerCase.endsWith( QLatin1String( ".gif" ) )
	             || lowerCase.endsWith( QLatin1String( ".svg" ) ) )
		&& ( info.lastModified().secsTo( QDateTime::currentDateTime() )
		     > deleteOnlyFilesOlderThan ) )
	    {
		mDebug() << "FileStorageWatcher: Delete "
		         << filePath;
		m_filesDeleted++;
		m_currentCacheSize -= info.size();
		QFile::remove( filePath );
	    }
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
        m_dataDirectory = MarbleDirs::localPath() + "/cache/";
 
    if ( ! QDir( m_dataDirectory ).exists() ) 
        QDir::root().mkpath( m_dataDirectory );
    
    m_started = false;
    m_themeLimitMutex = new QMutex();
    
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
    
    delete m_themeLimitMutex;
}

void FileStorageWatcher::setCacheLimit( quint64 bytes )
{
    QMutexLocker locker( m_themeLimitMutex );
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

void FileStorageWatcher::updateTheme( const QString &mapTheme )
{
    QMutexLocker locker( m_themeLimitMutex );
    if( m_started )
	// This is done directly
	m_thread->updateTheme( mapTheme );
    // Save the theme, thread has to initialized with the right one.
    m_theme = mapTheme;
}

void FileStorageWatcher::run()
{
    m_thread = new FileStorageWatcherThread( m_dataDirectory );
    if( !m_quitting ) {
	m_themeLimitMutex->lock();
	m_thread->setCacheLimit( m_limit );
	m_thread->updateTheme( m_theme );
	m_started = true;
	mDebug() << m_started;
	m_themeLimitMutex->unlock();
	
	m_thread->getCurrentCacheSize();
	
	connect( this, SIGNAL( sizeChanged( qint64 ) ),
		 m_thread, SLOT( addToCurrentSize( qint64 ) ) );
	connect( this, SIGNAL( cleared() ),
		 m_thread, SLOT( resetCurrentSize() ) );
    
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

#include "FileStorageWatcher.moc"
