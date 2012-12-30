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
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtGui/QStandardItemModel>

// Local dir
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneParser.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"

namespace
{
    static const QString mapDirName = "maps";
    static const int columnRelativePath = 1;
}

namespace Marble
{

// The sole purpose of the class is to expose the protected
// setRoleNames method and use it later on
class StandardItemModelWithRoleNames: public QStandardItemModel
{
public:
    StandardItemModelWithRoleNames( int rows, int columns, QObject *parent = 0 );

    void setRoleNames( const QHash<int, QByteArray> &roleNames );
};

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
     * @brief Returns all directory paths and .dgml file paths below local and
     *        system map directory.
     */
    static QStringList pathsToWatch();

    /**
     * @brief Helper method for updateMapThemeModel().
     */
    QList<QStandardItem *> createMapThemeRow( const QString& mapThemeID );

    static void deleteDirectory( const QString& path );
    static void deleteDataDirectories( const QString& path );
    static void deletePreview( const QString& path );

    MapThemeManager *const q;
    StandardItemModelWithRoleNames m_mapThemeModel;
    QFileSystemWatcher m_fileSystemWatcher;
    bool m_isInitialized;
};

StandardItemModelWithRoleNames::StandardItemModelWithRoleNames( int rows, int columns, QObject *parent ) :
    QStandardItemModel( rows, columns, parent )
{
    // nothing to do
}

void StandardItemModelWithRoleNames::setRoleNames( const QHash<int, QByteArray> &roleNames )
{
    QStandardItemModel::setRoleNames( roleNames );
}

MapThemeManager::Private::Private( MapThemeManager *parent )
    : q( parent ),
      m_mapThemeModel( 0, 3 ),
      m_fileSystemWatcher(),
      m_isInitialized( false )
{
    QHash<int,QByteArray> roleNames = m_mapThemeModel.roleNames();
    roleNames[ Qt::DecorationRole ] = "icon";
    roleNames[ Qt::UserRole + 1 ] = "mapThemeId";
    m_mapThemeModel.setRoleNames( roleNames );
}

MapThemeManager::Private::~Private()
{
}


MapThemeManager::MapThemeManager( QObject *parent )
    : QObject( parent ),
      d( new Private( this ) )
{
    const QStringList paths = d->pathsToWatch();
    d->m_fileSystemWatcher.addPaths( paths );
    connect( &d->m_fileSystemWatcher, SIGNAL( directoryChanged( const QString& )),
             this, SLOT( directoryChanged( const QString& )));
    connect( &d->m_fileSystemWatcher, SIGNAL( fileChanged( const QString& )),
             this, SLOT( fileChanged( const QString& )));
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

GeoSceneDocument* MapThemeManager::loadMapTheme( const QString& mapThemeStringID ) const
{
    if ( mapThemeStringID.isEmpty() )
        return 0;

    return Private::loadMapThemeFile( mapThemeStringID );
}

void MapThemeManager::deleteMapTheme( const QString &mapThemeId )
{
    QDir mapThemeDir( QFileInfo( MarbleDirs::localPath() + "/maps/" + mapThemeId ).path() );
    Private::deleteDirectory( mapThemeDir.path() + "/legend/" );
    Private::deleteDataDirectories( mapThemeDir.path() + '/' );
    Private::deletePreview( mapThemeDir.path() + '/' );
    QFile( MarbleDirs::localPath() + "/maps/" + mapThemeId ).remove();
    QFile( mapThemeDir.path() + "/legend.html" ).remove();
    QDir().rmdir( mapThemeDir.path() );
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

QList<QStandardItem *> MapThemeManager::Private::createMapThemeRow( QString const& mapThemeID )
{
    QList<QStandardItem *> itemList;

    GeoSceneDocument *mapTheme = loadMapThemeFile( mapThemeID );
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

    itemList << item;
    itemList << new QStandardItem( mapTheme->head()->target() + '/'
                                   + mapTheme->head()->theme() + '/'
                                   + mapTheme->head()->theme() + ".dgml" );
    itemList << new QStandardItem( QObject::tr( description.toUtf8() ) );

    delete mapTheme;

    return itemList;
}

void MapThemeManager::Private::updateMapThemeModel()
{
    mDebug() << "updateMapThemeModel";
    m_mapThemeModel.clear();

    m_mapThemeModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    m_mapThemeModel.setHeaderData(1, Qt::Horizontal, QObject::tr("Path"));
    m_mapThemeModel.setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));

    QStringList stringlist = findMapThemes();
    QStringListIterator it( stringlist );

    while ( it.hasNext() ) {
        QString mapThemeID = it.next();

    	QList<QStandardItem *> itemList = createMapThemeRow( mapThemeID );
        if ( !itemList.empty() ) {
            m_mapThemeModel.appendRow( itemList );
        }
    }
}

void MapThemeManager::Private::directoryChanged( const QString& path )
{
    mDebug() << "directoryChanged:" << path;

    QStringList paths = Private::pathsToWatch();
    m_fileSystemWatcher.addPaths( paths );

    mDebug() << "Emitting themesChanged()";
    emit q->themesChanged();
    updateMapThemeModel();
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

void MapThemeManager::Private::deleteDirectory( const QString& path )
{
    QDir directory( path );
    foreach( const QString &filename, directory.entryList( QDir::Files | QDir::NoDotAndDotDot ) )
        QFile( path + filename ).remove();
    QDir().rmdir( path );
}

void MapThemeManager::Private::deleteDataDirectories( const QString& path )
{
    QDir directoryv( path );
    foreach( const QString &filename, directoryv.entryList( QDir::AllEntries | QDir::NoDotAndDotDot ) )
    {
        QString filepath = path + '/' + filename;
        QFile file( filepath );
        if( QFileInfo( filepath ).isDir() && filename.contains( QRegExp( "^[0-9]+$" ) ) )
        {
            deleteDataDirectories( filepath );
            QDir().rmdir( filepath );
        }
        else if( filename.contains( QRegExp( "^[0-9]\\..+" ) ) )
            file.remove();
    }
}

void MapThemeManager::Private::deletePreview( const QString& path )
{
    QDir directoryv( path, "preview.*" );
    foreach( const QString &filename, directoryv.entryList() )
        QFile( path + '/' + filename ).remove();
}

}

#include "MapThemeManager.moc"
