//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//


// Own
#include "MapThemeManager.h"

// Qt
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QStandardItemModel>

// Local dir
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneParser.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "Planet.h"

namespace
{
    static const QString mapDirName = "maps";
    static const int columnRelativePath = 1;
}

namespace Marble
{

class MapThemeManager::Private
{
public:
    Private( MapThemeManager *parent );
    ~Private();

    void directoryChanged( const QString& path );
    void fileChanged( const QString & path );

    /**
     * @brief Updates the map theme model on request.
     *
     * This method should usually get invoked on startup or
     * by a QFileSystemWatcher instance.
     */
    void updateMapThemeModel();

    void watchPaths();

    /**
     * @brief Adds directory paths and .dgml file paths to the given QStringList.
     */
    static void addMapThemePaths( const QString& mapPathName, QStringList& result );

    /**
     * @brief Helper method for findMapThemes(). Searches for .dgml files below
     *        given directory path.
     */
    static QStringList findMapThemes( const QString& basePath );

    /**
     * @brief Searches for .dgml files below local and system map directory.
     */
    static QStringList findMapThemes();

    static GeoSceneDocument* loadMapThemeFile( const QString& mapThemeId );

    /**
     * @brief Helper method for updateMapThemeModel().
     */
    static QList<QStandardItem *> createMapThemeRow( const QString& mapThemeID );

    /**
     * @brief Deletes any directory with its contents.
     * @param directory Path to directory
     * WARNING: Please do not raise this method's visibility in future, keep it private.
     */
    static bool deleteDirectory( const QString &directory );

    MapThemeManager *const q;
    QStandardItemModel m_mapThemeModel;
    QStandardItemModel m_celestialList;
    QFileSystemWatcher m_fileSystemWatcher;
    bool m_isInitialized;

private:
    /**
     * @brief Returns all directory paths and .dgml file paths below local and
     *        system map directory.
     */
    QStringList pathsToWatch();
};

MapThemeManager::Private::Private( MapThemeManager *parent )
    : q( parent ),
      m_mapThemeModel( 0, 3 ),
      m_celestialList(),
      m_fileSystemWatcher(),
      m_isInitialized( false )
{
}

MapThemeManager::Private::~Private()
{
}


MapThemeManager::MapThemeManager( QObject *parent )
    : QObject( parent ),
      d( new Private( this ) )
{
    d->watchPaths();
    connect( &d->m_fileSystemWatcher, SIGNAL(directoryChanged(QString)),
             this, SLOT(directoryChanged(QString)));
    connect( &d->m_fileSystemWatcher, SIGNAL(fileChanged(QString)),
             this, SLOT(fileChanged(QString)));
}

MapThemeManager::~MapThemeManager()
{
    delete d;
}

QStringList MapThemeManager::mapThemeIds() const
{
    QStringList result;

    if ( !d->m_isInitialized ) {
        d->updateMapThemeModel();
        d->m_isInitialized = true;
    }

    for( int i = 0; i < d->m_mapThemeModel.rowCount(); ++i ) {
        const QString id = d->m_mapThemeModel.data( d->m_mapThemeModel.index( i, 0 ), Qt::UserRole + 1 ).toString();
        result << id;
    }

    return result;
}

GeoSceneDocument* MapThemeManager::loadMapTheme( const QString& mapThemeStringID )
{
    if ( mapThemeStringID.isEmpty() )
        return 0;

    return Private::loadMapThemeFile( mapThemeStringID );
}

void MapThemeManager::deleteMapTheme( const QString &mapThemeId )
{
    QString dgmlPath = MarbleDirs::localPath() + "/maps/" + mapThemeId;
    QFileInfo dgmlFile(dgmlPath);
    
    QString themeDir = dgmlFile.dir().absolutePath();
    Private::deleteDirectory( themeDir );
}

bool MapThemeManager::Private::deleteDirectory( const QString& directory )
{
    QDir dir( directory );
    bool result = true;
 
    if ( dir.exists() ) {
        Q_FOREACH( const QFileInfo &info, dir.entryInfoList(
            QDir::NoDotAndDotDot | QDir::System | QDir::Hidden |
            QDir::AllDirs | QDir::Files,
            QDir::DirsFirst ) ) {
            
            if ( info.isDir() ) {
                result = deleteDirectory( info.absoluteFilePath() );
            } else {
                result = QFile::remove( info.absoluteFilePath() );
            }
 
            if ( !result ) {
                return result;
            }
        }
        
        result = dir.rmdir( directory );
        
        if( !result ) {
            return result;
        }
    }
    
    return result;
}

GeoSceneDocument* MapThemeManager::Private::loadMapThemeFile( const QString& mapThemeStringID )
{
    const QString mapThemePath = mapDirName + '/' + mapThemeStringID;
    const QString dgmlPath = MarbleDirs::path( mapThemePath );

    // Check whether file exists
    QFile file( dgmlPath );
    if ( !file.exists() ) {
        qWarning() << "Map theme file does not exist:" << dgmlPath;
        return 0;
    }

    // Open file in right mode
    const bool fileReadable = file.open( QIODevice::ReadOnly );

    if ( !fileReadable ) {
        qWarning() << "Map theme file not readable:" << dgmlPath;
        return 0;
    }

    GeoSceneParser parser( GeoScene_DGML );

    if ( !parser.read( &file )) {
        qWarning() << "Map theme file not well-formed:" << dgmlPath;
        return 0;
    }

    mDebug() << "Map theme file successfully loaded:" << dgmlPath;

    // Get result document
    GeoSceneDocument* document = static_cast<GeoSceneDocument*>( parser.releaseDocument() );
    Q_ASSERT( document );
    return document;
}

QStringList MapThemeManager::Private::pathsToWatch()
{
    QStringList result;
    const QString localMapPathName = MarbleDirs::localPath() + '/' + mapDirName;
    const QString systemMapPathName = MarbleDirs::systemPath() + '/' + mapDirName;

    if( !QDir().exists( localMapPathName ) ) {
        QDir().mkpath( localMapPathName );
    }

    result << localMapPathName;
    result << systemMapPathName;
    addMapThemePaths( localMapPathName, result );
    addMapThemePaths( systemMapPathName, result );
    return result;
}

QStringList MapThemeManager::Private::findMapThemes( const QString& basePath )
{
    const QString mapPathName = basePath + '/' + mapDirName;

    QDir paths = QDir( mapPathName );

    QStringList mapPaths = paths.entryList( QStringList( "*" ),
                                            QDir::AllDirs
                                            | QDir::NoSymLinks
                                            | QDir::NoDotAndDotDot );
    QStringList mapDirs;

    for ( int planet = 0; planet < mapPaths.size(); ++planet ) {
        QDir themeDir = QDir( mapPathName + '/' + mapPaths.at( planet ) );
        QStringList themeMapPaths = themeDir.entryList( 
                                     QStringList( "*" ),
                                     QDir::AllDirs |
                                     QDir::NoSymLinks |
                                     QDir::NoDotAndDotDot );
        for ( int theme = 0; theme < themeMapPaths.size(); ++theme ) {
            mapDirs << mapPathName + '/' + mapPaths.at( planet ) + '/'
                + themeMapPaths.at( theme );
        }
    }

    QStringList mapFiles;
    QStringListIterator it( mapDirs );
    while ( it.hasNext() ) {
        QString themeDir = it.next() + '/';
        QString themeDirName = QDir( themeDir ).path().section( '/', -2, -1 );
        QStringList tmp = QDir( themeDir ).entryList( QStringList( "*.dgml" ),
                                                      QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator k( tmp );
            while ( k.hasNext() ) {
                QString themeXml = k.next();
                mapFiles << themeDirName + '/' + themeXml;
            }
        }
    }

    return mapFiles;
}

QStringList MapThemeManager::Private::findMapThemes()
{
    QStringList mapFilesLocal = findMapThemes( MarbleDirs::localPath() );
    QStringList mapFilesSystem = findMapThemes( MarbleDirs::systemPath() );
    QStringList allMapFiles( mapFilesLocal );
    allMapFiles << mapFilesSystem;

    // remove duplicate entries
    allMapFiles.sort();
    for ( int i = 1; i < allMapFiles.size(); ++i ) {
        if ( allMapFiles.at(i) == allMapFiles.at( i-1 ) ) {
            allMapFiles.removeAt( i );
            --i;
        }
    }

    return allMapFiles;
}

QStandardItemModel* MapThemeManager::mapThemeModel()
{
    if ( !d->m_isInitialized ) {
        d->updateMapThemeModel();
        d->m_isInitialized = true;
    }
    return &d->m_mapThemeModel;
}

QStandardItemModel *MapThemeManager::celestialBodiesModel()
{
    if ( !d->m_isInitialized ) {
        d->updateMapThemeModel();
        d->m_isInitialized = true;
    }

    return &d->m_celestialList;
}

QList<QStandardItem *> MapThemeManager::Private::createMapThemeRow( QString const& mapThemeID )
{
    QList<QStandardItem *> itemList;

    QScopedPointer<GeoSceneDocument> mapTheme( loadMapThemeFile( mapThemeID ) );
    if ( !mapTheme || !mapTheme->head()->visible() ) {
        return itemList;
    }

    QPixmap themeIconPixmap;
    QString relativePath;

    relativePath = mapDirName + '/'
        + mapTheme->head()->target() + '/' + mapTheme->head()->theme() + '/'
        + mapTheme->head()->icon()->pixmap();
    themeIconPixmap.load( MarbleDirs::path( relativePath ) );

    if ( themeIconPixmap.isNull() ) {
        relativePath = "svg/application-x-marble-gray.png"; 
        themeIconPixmap.load( MarbleDirs::path( relativePath ) );
    }
    else {
        // Make sure we don't keep excessively large previews in memory
        // TODO: Scale the icon down to the default icon size in MarbleSelectView.
        //       For now maxIconSize already equals what's expected by the listview.
        QSize maxIconSize( 136, 136 );
        if ( themeIconPixmap.size() != maxIconSize ) {
            mDebug() << "Smooth scaling theme icon";
            themeIconPixmap = themeIconPixmap.scaled( maxIconSize,
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation );
        }
    }

    QIcon mapThemeIcon =  QIcon( themeIconPixmap );

    QString name = mapTheme->head()->name();
    QString description = mapTheme->head()->description();

    QStandardItem *item = new QStandardItem( name );
    item->setData( QObject::tr( name.toUtf8() ), Qt::DisplayRole );
    item->setData( mapThemeIcon, Qt::DecorationRole );
    item->setData( QString( "<span style=\" max-width: 150 px;\"> "
                            + QObject::tr( description.toUtf8() ) + " </span>" ), Qt::ToolTipRole );
    item->setData( mapThemeID, Qt::UserRole + 1 );
    item->setData( QObject::tr( description.toUtf8() ), Qt::UserRole + 2 );

    itemList << item;

    return itemList;
}

void MapThemeManager::Private::updateMapThemeModel()
{
    mDebug() << "updateMapThemeModel";
    m_mapThemeModel.clear();

    m_mapThemeModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));

    QStringList stringlist = findMapThemes();
    QStringListIterator it( stringlist );

    while ( it.hasNext() ) {
        QString mapThemeID = it.next();

    	QList<QStandardItem *> itemList = createMapThemeRow( mapThemeID );
        if ( !itemList.empty() ) {
            m_mapThemeModel.appendRow( itemList );
        }
    }

    foreach ( const QString &mapThemeId, stringlist ) {
        QString celestialBodyId = mapThemeId.section( '/', 0, 0 );
        QString celestialBodyName = Planet::name( celestialBodyId );

        QList<QStandardItem*> matchingItems = m_celestialList.findItems( celestialBodyId, Qt::MatchExactly, 1 );
        if ( matchingItems.isEmpty() ) {
            m_celestialList.appendRow( QList<QStandardItem*>()
                                << new QStandardItem( celestialBodyName )
                                << new QStandardItem( celestialBodyId ) );
        }
    }
}

void MapThemeManager::Private::watchPaths()
{
    QStringList const paths = pathsToWatch();
    QStringList const files = m_fileSystemWatcher.files();
    QStringList const directories = m_fileSystemWatcher.directories();
    // Check each resource to add that it is not being watched already,
    // otherwise some qWarning appears
    foreach( const QString &resource, paths ) {
        if ( !directories.contains( resource ) && !files.contains( resource ) ) {
            m_fileSystemWatcher.addPath( resource );
        }
    }
}

void MapThemeManager::Private::directoryChanged( const QString& path )
{
    mDebug() << "directoryChanged:" << path;
    watchPaths();

    mDebug() << "Emitting themesChanged()";
    updateMapThemeModel();
    emit q->themesChanged();
}

void MapThemeManager::Private::fileChanged( const QString& path )
{
    mDebug() << "fileChanged:" << path;

    // 1. if the file does not (anymore) exist, it got deleted and we
    //    have to delete the corresponding item from the model
    // 2. if the file exists it is changed and we have to replace
    //    the item with a new one.

    QString mapThemeId = path.section( '/', -3 );
    mDebug() << "mapThemeId:" << mapThemeId;
    QList<QStandardItem *> matchingItems = m_mapThemeModel.findItems( mapThemeId,
                                                                          Qt::MatchFixedString
                                                                          | Qt::MatchCaseSensitive,
                                                                          columnRelativePath );
    mDebug() << "matchingItems:" << matchingItems.size();
    Q_ASSERT( matchingItems.size() <= 1 );
    int insertAtRow = 0;

    if ( matchingItems.size() == 1 ) {
        const int row = matchingItems.front()->row();
	insertAtRow = row;
        QList<QStandardItem *> toBeDeleted = m_mapThemeModel.takeRow( row );
	while ( !toBeDeleted.isEmpty() ) {
            delete toBeDeleted.takeFirst();
        }
    }

    QFileInfo fileInfo( path );
    if ( fileInfo.exists() ) {
        QList<QStandardItem *> newMapThemeRow = createMapThemeRow( mapThemeId );
        if ( !newMapThemeRow.empty() ) {
            m_mapThemeModel.insertRow( insertAtRow, newMapThemeRow );
        }
    }
    
    emit q->themesChanged();
}

//
// <mapPathName>/<orbDirName>/<themeDirName>
//
void MapThemeManager::Private::addMapThemePaths( const QString& mapPathName, QStringList& result )
{
    QDir mapPath( mapPathName );
    QStringList orbDirNames = mapPath.entryList( QStringList( "*" ),
                                                 QDir::AllDirs
                                                 | QDir::NoSymLinks
                                                 | QDir::NoDotAndDotDot );
    QStringListIterator itOrb( orbDirNames );
    while ( itOrb.hasNext() ) {
        QString orbPathName = mapPathName + '/' + itOrb.next();
        result << orbPathName;

        QDir orbPath( orbPathName );
        QStringList themeDirNames = orbPath.entryList( QStringList( "*" ),
                                                       QDir::AllDirs
                                                       | QDir::NoSymLinks
                                                       | QDir::NoDotAndDotDot );
        QStringListIterator itThemeDir( themeDirNames );
        while ( itThemeDir.hasNext() ) {
            QString themePathName = orbPathName + '/' + itThemeDir.next();
            result << themePathName;

            QDir themePath( themePathName );
	    QStringList themeFileNames = themePath.entryList( QStringList( "*.dgml" ),
                                                              QDir::Files
                                                              | QDir::NoSymLinks );
            QStringListIterator itThemeFile( themeFileNames );
            while ( itThemeFile.hasNext() ) {
                QString themeFilePathName = themePathName + '/' + itThemeFile.next();
                result << themeFilePathName;
            }
        }
    }
}

}

#include "MapThemeManager.moc"
