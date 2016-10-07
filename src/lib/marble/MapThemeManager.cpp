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
#include <QStandardItemModel>

// Local dir
#include "GeoDataPhotoOverlay.h"
#include "GeoSceneDocument.h"
#include "GeoSceneMap.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneParser.h"
#include "GeoSceneLayer.h"
#include "GeoSceneTileDataset.h"
#include "GeoSceneTextureTileDataset.h"
#include "GeoSceneProperty.h"
#include "GeoSceneZoom.h"
#include "GeoSceneSettings.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "Planet.h"
#include "PlanetFactory.h"

// Std
#include <limits>

namespace
{
    static const QString mapDirName = "maps";
    static const int columnRelativePath = 1;
}

namespace Marble
{

class Q_DECL_HIDDEN MapThemeManager::Private
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
    static QStringList pathsToWatch();
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

    const int mapThemeIdCount = d->m_mapThemeModel.rowCount();
    result.reserve(mapThemeIdCount);
    for (int i = 0; i < mapThemeIdCount; ++i) {
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
    const QString dgmlPath = MarbleDirs::localPath() + QLatin1String("/maps/") + mapThemeId;
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
    const QString mapThemePath = mapDirName + QLatin1Char('/') + mapThemeStringID;
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
    const QString localMapPathName = MarbleDirs::localPath() + QLatin1Char('/') + mapDirName;
    const QString systemMapPathName = MarbleDirs::systemPath() + QLatin1Char('/') + mapDirName;

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
    const QString mapPathName = basePath + QLatin1Char('/') + mapDirName;

    QDir paths = QDir( mapPathName );

    QStringList mapPaths = paths.entryList( QStringList( "*" ),
                                            QDir::AllDirs
                                            | QDir::NoSymLinks
                                            | QDir::NoDotAndDotDot );
    QStringList mapDirs;

    for ( int planet = 0; planet < mapPaths.size(); ++planet ) {
        QDir themeDir = QDir(mapPathName + QLatin1Char('/') + mapPaths.at(planet));
        QStringList themeMapPaths = themeDir.entryList(
                                     QStringList( "*" ),
                                     QDir::AllDirs |
                                     QDir::NoSymLinks |
                                     QDir::NoDotAndDotDot );
        for ( int theme = 0; theme < themeMapPaths.size(); ++theme ) {
            mapDirs << mapPathName + QLatin1Char('/') + mapPaths.at(planet) + QLatin1Char('/')
                + themeMapPaths.at( theme );
        }
    }

    QStringList mapFiles;
    QStringListIterator it( mapDirs );
    while ( it.hasNext() ) {
        QString themeDir = it.next() + QLatin1Char('/');
        QString themeDirName = QDir(themeDir).path().section(QLatin1Char('/'), -2, -1);
        QStringList tmp = QDir( themeDir ).entryList( QStringList( "*.dgml" ),
                                                      QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator k( tmp );
            while ( k.hasNext() ) {
                QString themeXml = k.next();
                mapFiles << themeDirName + QLatin1Char('/') + themeXml;
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

    QString relativePath = mapDirName + QLatin1Char('/')
        + mapTheme->head()->target() + QLatin1Char('/') + mapTheme->head()->theme() + QLatin1Char('/')
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
    const QString translatedDescription = QCoreApplication::translate("DGML", mapTheme->head()->description().toUtf8().constData());
    const QString toolTip = QLatin1String("<span style=\" max-width: 150 px;\"> ") + translatedDescription + QLatin1String(" </span>");

    QStandardItem *item = new QStandardItem( name );
    item->setData(QCoreApplication::translate("DGML", name.toUtf8().constData()), Qt::DisplayRole);
    item->setData( mapThemeIcon, Qt::DecorationRole );
    item->setData(toolTip, Qt::ToolTipRole);
    item->setData( mapThemeID, Qt::UserRole + 1 );
    item->setData(translatedDescription, Qt::UserRole + 2);

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
        const QString celestialBodyId = mapThemeId.section(QLatin1Char('/'), 0, 0);
        QString celestialBodyName = PlanetFactory::localizedName( celestialBodyId );

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

    const QString mapThemeId = path.section(QLatin1Char('/'), -3);
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
        const QString orbPathName = mapPathName + QLatin1Char('/') + itOrb.next();
        result << orbPathName;

        QDir orbPath( orbPathName );
        QStringList themeDirNames = orbPath.entryList( QStringList( "*" ),
                                                       QDir::AllDirs
                                                       | QDir::NoSymLinks
                                                       | QDir::NoDotAndDotDot );
        QStringListIterator itThemeDir( themeDirNames );
        while ( itThemeDir.hasNext() ) {
            const QString themePathName = orbPathName + QLatin1Char('/') + itThemeDir.next();
            result << themePathName;

            QDir themePath( themePathName );
	    QStringList themeFileNames = themePath.entryList( QStringList( "*.dgml" ),
                                                              QDir::Files
                                                              | QDir::NoSymLinks );
            QStringListIterator itThemeFile( themeFileNames );
            while ( itThemeFile.hasNext() ) {
                const QString themeFilePathName = themePathName + QLatin1Char('/') + itThemeFile.next();
                result << themeFilePathName;
            }
        }
    }
}

GeoSceneDocument *MapThemeManager::createMapThemeFromOverlay( const GeoDataPhotoOverlay *overlayData )
{
    GeoSceneDocument * document = new GeoSceneDocument();
    document->head()->setDescription( overlayData->description() );
    document->head()->setName( overlayData->name() );
    document->head()->setTheme( "photo" );
    document->head()->setTarget( "panorama" );
    document->head()->setRadius(36000);
    document->head()->setVisible(true);

    document->head()->zoom()->setMaximum(3500);
    document->head()->zoom()->setMinimum(900);
    document->head()->zoom()->setDiscrete(false);

    GeoSceneLayer * layer = new GeoSceneLayer( "photo" );
    layer->setBackend("texture");

    GeoSceneTextureTileDataset * texture = new GeoSceneTextureTileDataset( "map" );
    texture->setExpire(std::numeric_limits<int>::max());

    QString fileName = overlayData->absoluteIconFile();
    QFileInfo fileInfo( fileName );
    fileName = fileInfo.fileName();

    QString sourceDir = fileInfo.absoluteDir().path();

    QString extension = fileInfo.suffix();

    texture->setSourceDir( sourceDir );
    texture->setFileFormat( extension );
    texture->setInstallMap( fileName );
    texture->setTileProjection(GeoSceneAbstractTileProjection::Equirectangular);

    layer->addDataset(texture);

    document->map()->addLayer(layer);

    GeoSceneSettings *settings = document->settings();

    GeoSceneProperty *gridProperty = new GeoSceneProperty( "coordinate-grid" );
    gridProperty->setValue( false );
    gridProperty->setAvailable( false );
    settings->addProperty( gridProperty );

    GeoSceneProperty *overviewmap = new GeoSceneProperty( "overviewmap" );
    overviewmap->setValue( false );
    overviewmap->setAvailable( false );
    settings->addProperty( overviewmap );

    GeoSceneProperty *compass = new GeoSceneProperty( "compass" );
    compass->setValue( false );
    compass->setAvailable( false );
    settings->addProperty( compass );

    GeoSceneProperty *scalebar = new GeoSceneProperty( "scalebar" );
    scalebar->setValue( true );
    scalebar->setAvailable( true );
    settings->addProperty( scalebar );

    return document;
}

}

#include "moc_MapThemeManager.cpp"
