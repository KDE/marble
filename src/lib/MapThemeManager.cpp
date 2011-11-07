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

class MapThemeManagerPrivate
{
public:
    MapThemeManagerPrivate();
    ~MapThemeManagerPrivate();
    StandardItemModelWithRoleNames* m_mapThemeModel;
    QFileSystemWatcher* m_fileSystemWatcher;
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

MapThemeManagerPrivate::MapThemeManagerPrivate()
    : m_mapThemeModel( new StandardItemModelWithRoleNames( 0, 3 ) ),
      m_fileSystemWatcher( new QFileSystemWatcher ),
      m_isInitialized( false )
{
    QHash<int,QByteArray> roleNames = m_mapThemeModel->roleNames();
    roleNames[ Qt::DecorationRole ] = "icon";
    roleNames[ Qt::UserRole + 1 ] = "mapThemeId";
    m_mapThemeModel->setRoleNames( roleNames );
}

MapThemeManagerPrivate::~MapThemeManagerPrivate()
{
    delete m_mapThemeModel;
    delete m_fileSystemWatcher;
}


MapThemeManager::MapThemeManager( QObject *parent )
    : QObject( parent ),
      d( new MapThemeManagerPrivate )
{
    initFileSystemWatcher();
}

MapThemeManager::~MapThemeManager()
{
    delete d;
}

QList<GeoSceneDocument const*> MapThemeManager::mapThemes() const
{
    QList<GeoSceneDocument const*> result;
    const QStringList mapThemes = findMapThemes();
    QStringList::const_iterator pos = mapThemes.constBegin();
    QStringList::const_iterator const end = mapThemes.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneDocument* document = loadMapTheme( *pos );
        if ( document ) {
            result.append( document );
        }
    }
    return result;
}

void MapThemeManager::initialize()
{
    // Delayed model initialization
    updateMapThemeModel();
    d->m_isInitialized = true;
}

void MapThemeManager::initFileSystemWatcher()
{
    const QStringList paths = pathsToWatch();
/*
    foreach(const QString& path, paths)
        mDebug() << "path to watch: " << path;
*/
    d->m_fileSystemWatcher->addPaths( paths );
    connect( d->m_fileSystemWatcher, SIGNAL( directoryChanged( const QString& )),
             this, SLOT( directoryChanged( const QString& )));
    connect( d->m_fileSystemWatcher, SIGNAL( fileChanged( const QString& )),
             this, SLOT( fileChanged( const QString& )));
}

GeoSceneDocument* MapThemeManager::loadMapTheme( const QString& mapThemeStringID )
{
    if ( mapThemeStringID.isEmpty() )
        return 0;

    mDebug() << "loadMapTheme" << mapThemeStringID;
    const QString mapThemePath = mapDirName + '/' + mapThemeStringID;
    return loadMapThemeFile( mapThemePath );
}

GeoSceneDocument* MapThemeManager::loadMapThemeFile( const QString& mapThemePath )
{
    // Check whether file exists
    QFile file( MarbleDirs::path( mapThemePath ) );
    if ( !file.exists() ) {
        mDebug() << "File does not exist:" << MarbleDirs::path( mapThemePath );
        return 0;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    GeoSceneParser parser( GeoScene_DGML );

    if ( !parser.read( &file )) {
        qDebug("Could not parse file!");
        return 0;
    }

    // Get result document
    GeoSceneDocument* document = static_cast<GeoSceneDocument*>( parser.releaseDocument() );
    Q_ASSERT( document );
    return document;
}

QStringList MapThemeManager::pathsToWatch()
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

QStringList MapThemeManager::findMapThemes( const QString& basePath )
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

QStringList MapThemeManager::findMapThemes()
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
        initialize();
    }
    return d->m_mapThemeModel;
}

QList<QStandardItem *> MapThemeManager::createMapThemeRow( QString const& mapThemeID )
{
    QList<QStandardItem *> itemList;

    GeoSceneDocument *mapTheme = loadMapTheme( mapThemeID );
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
    item->setData( tr( name.toUtf8() ), Qt::DisplayRole );
    item->setData( mapThemeIcon, Qt::DecorationRole );
    item->setData( QString( "<span style=\" max-width: 150 px;\"> "
                            + tr( description.toUtf8() ) + " </span>" ), Qt::ToolTipRole );
    item->setData( mapThemeID, Qt::UserRole + 1 );

    itemList << item;
    itemList << new QStandardItem( mapTheme->head()->target() + '/'
                                   + mapTheme->head()->theme() + '/'
                                   + mapTheme->head()->theme() + ".dgml" );
    itemList << new QStandardItem( tr( description.toUtf8() ) );

    delete mapTheme;

    return itemList;
}

void MapThemeManager::updateMapThemeModel()
{
    mDebug() << "updateMapThemeModel";
    d->m_mapThemeModel->clear();

    d->m_mapThemeModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    d->m_mapThemeModel->setHeaderData(1, Qt::Horizontal, tr("Path"));
    d->m_mapThemeModel->setHeaderData(2, Qt::Horizontal, tr("Description"));

    QStringList stringlist = findMapThemes();
    QStringListIterator it( stringlist );

    while ( it.hasNext() ) {
        QString mapThemeID = it.next();

    	QList<QStandardItem *> itemList = createMapThemeRow( mapThemeID );
        if ( !itemList.empty() ) {
            d->m_mapThemeModel->appendRow( itemList );
        }
    }
}

void MapThemeManager::directoryChanged( const QString& path )
{
    mDebug() << "directoryChanged:" << path;

    QStringList paths = pathsToWatch();
    d->m_fileSystemWatcher->addPaths( paths );

    mDebug() << "Emitting themesChanged()";
    emit themesChanged();
    updateMapThemeModel();
}

void MapThemeManager::fileChanged( const QString& path )
{
    mDebug() << "fileChanged:" << path;

    // 1. if the file does not (anymore) exist, it got deleted and we
    //    have to delete the corresponding item from the model
    // 2. if the file exists it is changed and we have to replace
    //    the item with a new one.

    QString mapThemeId = path.section( '/', -3 );
    mDebug() << "mapThemeId:" << mapThemeId;
    QList<QStandardItem *> matchingItems = d->m_mapThemeModel->findItems( mapThemeId,
                                                                          Qt::MatchFixedString
                                                                          | Qt::MatchCaseSensitive,
                                                                          columnRelativePath );
    mDebug() << "matchingItems:" << matchingItems.size();
    Q_ASSERT( matchingItems.size() <= 1 );
    int insertAtRow = 0;

    if ( matchingItems.size() == 1 ) {
        const int row = matchingItems.front()->row();
	insertAtRow = row;
        QList<QStandardItem *> toBeDeleted = d->m_mapThemeModel->takeRow( row );
	while ( !toBeDeleted.isEmpty() ) {
            delete toBeDeleted.takeFirst();
        }
    }

    QFileInfo fileInfo( path );
    if ( fileInfo.exists() ) {
        QList<QStandardItem *> newMapThemeRow = createMapThemeRow( mapThemeId );
        if ( !newMapThemeRow.empty() ) {
            d->m_mapThemeModel->insertRow( insertAtRow, newMapThemeRow );
        }
    }
    
    emit themesChanged();
}

//
// <mapPathName>/<orbDirName>/<themeDirName>
//
void MapThemeManager::addMapThemePaths( const QString& mapPathName, QStringList& result )
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
