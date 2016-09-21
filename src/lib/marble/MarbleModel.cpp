//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2010-2013  Bernhard Beschow  <bbeschow@cs.tu-berlin.de>
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "MarbleModel.h"

#include <cmath>

#include <QAtomicInt>
#include <QPointer>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QTextDocument>

#include "kdescendantsproxymodel.h"

#include "MapThemeManager.h"
#include "MarbleGlobal.h"
#include "MarbleDebug.h"

#include "GeoSceneDocument.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneHead.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoScenePalette.h"
#include "GeoSceneTileDataset.h"
#include "GeoSceneVector.h"

#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataLineStyle.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataTypes.h"

#include "DgmlAuxillaryDictionary.h"
#include "MarbleClock.h"
#include "FileStoragePolicy.h"
#include "FileStorageWatcher.h"
#include "PositionTracking.h"
#include "HttpDownloadManager.h"
#include "MarbleDirs.h"
#include "FileManager.h"
#include "GeoDataTreeModel.h"
#include "PlacemarkPositionProviderPlugin.h"
#include "Planet.h"
#include "PlanetFactory.h"
#include "PluginManager.h"
#include "StoragePolicy.h"
#include "SunLocator.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"
#include "routing/RoutingManager.h"
#include "RouteSimulationPositionProviderPlugin.h"
#include "BookmarkManager.h"
#include "ElevationModel.h"

namespace Marble
{

class MarbleModelPrivate
{
 public:
    MarbleModelPrivate()
        : m_clock(),
          m_planet(PlanetFactory::construct(QStringLiteral("earth"))),
          m_sunLocator( &m_clock, &m_planet ),
          m_pluginManager(),
          m_homePoint( -9.4, 54.8, 0.0, GeoDataCoordinates::Degree ),  // Some point that tackat defined. :-)
          m_homeZoom( 1050 ),
          m_mapTheme( 0 ),
          m_storagePolicy( MarbleDirs::localPath() ),
          m_downloadManager( &m_storagePolicy ),
          m_storageWatcher( MarbleDirs::localPath() ),
          m_treeModel(),
          m_descendantProxy(),
          m_placemarkProxyModel(),
          m_placemarkSelectionModel( 0 ),
          m_fileManager( &m_treeModel, &m_pluginManager ),
          m_positionTracking( &m_treeModel ),
          m_trackedPlacemark( 0 ),
          m_bookmarkManager( &m_treeModel ),
          m_routingManager( 0 ),
          m_legend( 0 ),
          m_workOffline( false ),
          m_elevationModel( &m_downloadManager, &m_pluginManager )
    {
        m_descendantProxy.setSourceModel( &m_treeModel );

        m_placemarkProxyModel.setFilterFixedString( GeoDataTypes::GeoDataPlacemarkType );
        m_placemarkProxyModel.setFilterKeyColumn( 1 );
        m_placemarkProxyModel.setSourceModel( &m_descendantProxy );

        m_groundOverlayProxyModel.setFilterFixedString( GeoDataTypes::GeoDataGroundOverlayType );
        m_groundOverlayProxyModel.setFilterKeyColumn( 1 );
        m_groundOverlayProxyModel.setSourceModel( &m_descendantProxy );
    }

    ~MarbleModelPrivate()
    {
        delete m_mapTheme;
        delete m_legend;
    }

    /**
     * @brief When applying a new theme, if the old theme
     * contains any data whose source file is same
     * as any of the source file in new theme, don't parse
     * the source file again. Instead just update the
     * styling info, based on <brush> and <pen>
     * value for that source file in new theme, in
     * the already parsed data. If the <brush> element
     * in new theme has some value for colorMap attribute
     * then we go for assignFillColors() which assigns each
     * placemark an inline style based on colors specified
     * in colorMap attribute. This avoid extra CPU
     * load of parsing the data file again.
     * @see assignFillColors()
     */
    void assignNewStyle(const QString &filePath, const GeoDataStyle::Ptr &style );

    /**
     * @brief Assigns each placemark an inline
     * style based on the color values specified
     * by colorMap attribute under <brush> element
     * in theme file.
     */
    void assignFillColors( const QString &filePath );

    void addHighlightStyle( GeoDataDocument *doc );

    // Misc stuff.
    MarbleClock              m_clock;
    Planet                   m_planet;
    SunLocator               m_sunLocator;

    PluginManager            m_pluginManager;

    // The home position
    GeoDataCoordinates       m_homePoint;
    int                      m_homeZoom;

    // View and paint stuff
    GeoSceneDocument        *m_mapTheme;

    FileStoragePolicy        m_storagePolicy;
    HttpDownloadManager      m_downloadManager;

    // Cache related
    FileStorageWatcher       m_storageWatcher;

    // Places on the map
    GeoDataTreeModel         m_treeModel;
    KDescendantsProxyModel   m_descendantProxy;
    QSortFilterProxyModel    m_placemarkProxyModel;
    QSortFilterProxyModel    m_groundOverlayProxyModel;

    // Selection handling
    QItemSelectionModel      m_placemarkSelectionModel;

    FileManager              m_fileManager;

    //Gps Stuff
    PositionTracking         m_positionTracking;

    const GeoDataPlacemark  *m_trackedPlacemark;

    BookmarkManager          m_bookmarkManager;
    RoutingManager          *m_routingManager;
    QTextDocument           *m_legend;

    bool                     m_workOffline;

    ElevationModel           m_elevationModel;
};

MarbleModel::MarbleModel( QObject *parent )
    : QObject( parent ),
      d( new MarbleModelPrivate() )
{
    // connect the StoragePolicy used by the download manager to the FileStorageWatcher
    connect( &d->m_storagePolicy, SIGNAL(cleared()),
             &d->m_storageWatcher, SLOT(resetCurrentSize()) );
    connect( &d->m_storagePolicy, SIGNAL(sizeChanged(qint64)),
             &d->m_storageWatcher, SLOT(addToCurrentSize(qint64)) );

    connect( &d->m_fileManager, SIGNAL(fileAdded(QString)),
             this, SLOT(assignFillColors(QString)) );

    d->m_routingManager = new RoutingManager( this, this );

    connect(&d->m_clock,   SIGNAL(timeChanged()),
            &d->m_sunLocator, SLOT(update()) );

    d->m_pluginManager.addPositionProviderPlugin( new PlacemarkPositionProviderPlugin( this ) );
    d->m_pluginManager.addPositionProviderPlugin( new RouteSimulationPositionProviderPlugin( this ) );
}

MarbleModel::~MarbleModel()
{
    delete d;

    mDebug() << "Model deleted:" << this;
}

BookmarkManager *MarbleModel::bookmarkManager()
{
    return &d->m_bookmarkManager;
}

QString MarbleModel::mapThemeId() const
{
    QString mapThemeId;

    if (d->m_mapTheme)
        mapThemeId = d->m_mapTheme->head()->mapThemeId();

    return mapThemeId;
}

GeoSceneDocument *MarbleModel::mapTheme()
{
    return d->m_mapTheme;
}

const GeoSceneDocument *MarbleModel::mapTheme() const
{
    return d->m_mapTheme;
}

// Set a particular theme for the map and load the appropriate tile level.
// If the tiles (for the lowest tile level) haven't been created already
// then create them here and now.
//
// FIXME: Move the tile creation dialogs out of this function.  Change
//        them into signals instead.
// FIXME: Get rid of 'currentProjection' here.  It's totally misplaced.
//

void MarbleModel::setMapThemeId( const QString &mapThemeId )
{
    if ( !mapThemeId.isEmpty() && mapThemeId == this->mapThemeId() )
        return;

    GeoSceneDocument *mapTheme = MapThemeManager::loadMapTheme( mapThemeId );
    setMapTheme( mapTheme );
}

void MarbleModel::setMapTheme( GeoSceneDocument *document )
{
    GeoSceneDocument *mapTheme = document;
    if ( !mapTheme ) {
        // Check whether the previous theme works
        if ( d->m_mapTheme ){
            qWarning() << "Selected theme doesn't work, so we stick to the previous one";
            return;
        }

        // Fall back to default theme
        QString defaultTheme = "earth/srtm/srtm.dgml";
        qWarning() << "Falling back to default theme:" << defaultTheme;
        mapTheme = MapThemeManager::loadMapTheme( defaultTheme );
    }

    // If this last resort doesn't work either shed a tear and exit
    if ( !mapTheme ) {
        qWarning() << "Couldn't find a valid DGML map.";
        return;
    }

    // find the list of previous theme's geodata
    QList<GeoSceneGeodata> currentDatasets;
    if ( d->m_mapTheme ) {
        foreach ( GeoSceneLayer *layer, d->m_mapTheme->map()->layers() ) {
            if ( layer->backend() != dgml::dgmlValue_geodata
                 && layer->backend() != dgml::dgmlValue_vector )
                continue;

            // look for documents
            foreach ( GeoSceneAbstractDataset *dataset, layer->datasets() ) {
                GeoSceneGeodata *data = dynamic_cast<GeoSceneGeodata*>( dataset );
                Q_ASSERT( data );
                currentDatasets << *data;
            }
        }
    }

    delete d->m_mapTheme;
    d->m_mapTheme = mapTheme;

    addDownloadPolicies( d->m_mapTheme );

    // Some output to show how to use this stuff ...
    mDebug() << "DGML2 Name       : " << d->m_mapTheme->head()->name();
/*
    mDebug() << "DGML2 Description: " << d->m_mapTheme->head()->description();

    if ( d->m_mapTheme->map()->hasTextureLayers() )
        mDebug() << "Contains texture layers! ";
    else
        mDebug() << "Does not contain any texture layers! ";

    mDebug() << "Number of SRTM textures: " << d->m_mapTheme->map()->layer("srtm")->datasets().count();

    if ( d->m_mapTheme->map()->hasVectorLayers() )
        mDebug() << "Contains vector layers! ";
    else
        mDebug() << "Does not contain any vector layers! ";
*/
    //Don't change the planet unless we have to...
    qreal const radiusAttributeValue = d->m_mapTheme->head()->radius();
    if( d->m_mapTheme->head()->target().toLower() != d->m_planet.id() || radiusAttributeValue != d->m_planet.radius() ) {
        mDebug() << "Changing Planet";
        d->m_planet = Magrathea::construct( d->m_mapTheme->head()->target().toLower() );
        if ( radiusAttributeValue > 0.0 ) {
            d->m_planet.setRadius( radiusAttributeValue );
        }
        sunLocator()->setPlanet( &d->m_planet );
    }

    QStringList fileList;
    QStringList propertyList;
    QList<GeoDataStyle::Ptr> styleList;
    QList<int> renderOrderList;

    bool skip = false;
    bool sourceFileMatch = false;
    int datasetIndex = -1;
    foreach ( GeoSceneLayer *layer, d->m_mapTheme->map()->layers() ) {
        if ( layer->backend() != dgml::dgmlValue_geodata
             && layer->backend() != dgml::dgmlValue_vector )
            continue;

        // look for datasets which are different from currentDatasets
        foreach ( const GeoSceneAbstractDataset *dataset, layer->datasets() ) {
            const GeoSceneGeodata *data = dynamic_cast<const GeoSceneGeodata*>( dataset );
            Q_ASSERT( data );
            skip = false;
            sourceFileMatch = false;
            for ( int i = 0; i < currentDatasets.size(); ++i ) {
                if ( currentDatasets[i] == *data ) {
                    currentDatasets.removeAt( i );
                    skip = true;
                    break;
                }
                /**
                 * If the sourcefile of data matches any in the currentDatasets then there
                 * is no need to parse the file again just update the style
                 * i.e. <brush> and <pen> values of already parsed file. assignNewStyle() does that
                 */
                if ( currentDatasets[i].sourceFile() == data->sourceFile() ) {
                    sourceFileMatch = true;
                    datasetIndex = i;
                }
            }
            if ( skip ) {
                continue;
            }

            QString filename = data->sourceFile();
            QString property = data->property();
            QPen pen = data->pen();
            QBrush brush = data->brush();
            GeoDataStyle::Ptr style;
            int renderOrder = data->renderOrder();

            /**
             * data->colors() are the colorMap values from dgml file. If this is not
             * empty then we are supposed to assign every placemark a different style
             * by giving it a color from colorMap values based on color index
             * of that placemark. See assignFillColors() for details. So, we need to
             * send an empty style to fileManeger otherwise the FileLoader::createFilterProperties()
             * will overwrite the parsed value of color index ( GeoDataPolyStyle::d->m_colorIndex ).
             */
            if ( data->colors().isEmpty() ) {
                GeoDataLineStyle lineStyle( pen.color() );
                lineStyle.setPenStyle( pen.style() );
                lineStyle.setWidth( pen.width() );
                GeoDataPolyStyle polyStyle( brush.color() );
                polyStyle.setFill( true );
                style = GeoDataStyle::Ptr(new GeoDataStyle);
                style->setLineStyle( lineStyle );
                style->setPolyStyle( polyStyle );
                style->setId(QStringLiteral("default"));
            }
            if ( sourceFileMatch && !currentDatasets[datasetIndex].colors().isEmpty() ) {
                /**
                 * if new theme file doesn't specify any colorMap for data
                 * then assignNewStyle otherwise assignFillColors.
                 */
                currentDatasets.removeAt( datasetIndex );
                if ( style ) {
                    qDebug() << "setMapThemeId-> color: " << style->polyStyle().color() << " file: " << filename;
                    d->assignNewStyle( filename, style );
                    style = GeoDataStyle::Ptr();
                }
                else {
                    d->assignFillColors( data->sourceFile() );
                }
            }
            else {
                fileList << filename;
                propertyList << property;
                styleList << style;
                renderOrderList << renderOrder;
            }
        }
    }
    // unload old currentDatasets which are not part of the new map
    foreach(const GeoSceneGeodata &data, currentDatasets) {
        d->m_fileManager.removeFile( data.sourceFile() );
    }
    // load new datasets
    for ( int i = 0 ; i < fileList.size(); ++i ) {
        d->m_fileManager.addFile( fileList.at(i), propertyList.at(i), styleList.at(i), MapDocument, renderOrderList.at(i) );
    }

    mDebug() << "THEME CHANGED: ***" << mapTheme->head()->mapThemeId();
    emit themeChanged( mapTheme->head()->mapThemeId() );
}

void MarbleModelPrivate::addHighlightStyle(GeoDataDocument* doc)
{
    if ( doc ) {
        /**
         * Add a highlight style to GeoDataDocument if
         *the theme file specifies any highlight color.
         */
        QColor highlightBrushColor = m_mapTheme->map()->highlightBrushColor();
        QColor highlightPenColor = m_mapTheme->map()->highlightPenColor();

        GeoDataStyle::Ptr highlightStyle(new GeoDataStyle);
        highlightStyle->setId(QStringLiteral("highlight"));

        if ( highlightBrushColor.isValid() ) {
            GeoDataPolyStyle highlightPolyStyle;
            highlightPolyStyle.setColor( highlightBrushColor );
            highlightPolyStyle.setFill( true );
            highlightStyle->setPolyStyle( highlightPolyStyle );
        }
        if ( highlightPenColor.isValid() ) {
            GeoDataLineStyle highlightLineStyle( highlightPenColor );
            highlightStyle->setLineStyle( highlightLineStyle );
        }
        if ( highlightBrushColor.isValid()
            || highlightPenColor.isValid() )
        {
            GeoDataStyleMap styleMap = doc->styleMap(QStringLiteral("default-map"));
            styleMap.insert(QStringLiteral("highlight"), QLatin1Char('#') + highlightStyle->id());
            doc->addStyle( highlightStyle );
            doc->addStyleMap( styleMap );
        }
    }
}

void MarbleModelPrivate::assignNewStyle( const QString &filePath, const GeoDataStyle::Ptr &style )
{
    GeoDataDocument *doc = m_fileManager.at( filePath );
    Q_ASSERT( doc );
    GeoDataStyleMap styleMap;
    styleMap.setId(QStringLiteral("default-map"));
    styleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + style->id());
    doc->addStyleMap( styleMap );
    doc->addStyle( style );

    addHighlightStyle( doc );

    const QString styleUrl = QLatin1Char('#') + styleMap.id();
    QVector<GeoDataFeature*>::iterator iter = doc->begin();
    QVector<GeoDataFeature*>::iterator const end = doc->end();

    for ( ; iter != end; ++iter ) {
        if ( (*iter)->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( *iter );
            if ( placemark ) {
                if ( placemark->geometry()->nodeType() != GeoDataTypes::GeoDataTrackType &&
                    placemark->geometry()->nodeType() != GeoDataTypes::GeoDataPointType )
                {
                    placemark->setStyleUrl(styleUrl);
                }
            }
        }
    }
}

void MarbleModel::home( qreal &lon, qreal &lat, int& zoom ) const
{
    d->m_homePoint.geoCoordinates( lon, lat, GeoDataCoordinates::Degree );
    zoom = d->m_homeZoom;
}

void MarbleModel::setHome( qreal lon, qreal lat, int zoom )
{
    d->m_homePoint = GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree );
    d->m_homeZoom = zoom;
    emit homeChanged( d->m_homePoint );
}

void MarbleModel::setHome( const GeoDataCoordinates& homePoint, int zoom )
{
    d->m_homePoint = homePoint;
    d->m_homeZoom = zoom;
    emit homeChanged( d->m_homePoint );
}

HttpDownloadManager *MarbleModel::downloadManager()
{
    return &d->m_downloadManager;
}

const HttpDownloadManager *MarbleModel::downloadManager() const
{
    return &d->m_downloadManager;
}


GeoDataTreeModel *MarbleModel::treeModel()
{
    return &d->m_treeModel;
}

const GeoDataTreeModel *MarbleModel::treeModel() const
{
    return &d->m_treeModel;
}

QAbstractItemModel *MarbleModel::placemarkModel()
{
    return &d->m_placemarkProxyModel;
}

const QAbstractItemModel *MarbleModel::placemarkModel() const
{
    return &d->m_placemarkProxyModel;
}

QAbstractItemModel *MarbleModel::groundOverlayModel()
{
    return &d->m_groundOverlayProxyModel;
}

const QAbstractItemModel *MarbleModel::groundOverlayModel() const
{
    return &d->m_groundOverlayProxyModel;
}

QItemSelectionModel *MarbleModel::placemarkSelectionModel()
{
    return &d->m_placemarkSelectionModel;
}

PositionTracking *MarbleModel::positionTracking() const
{
    return &d->m_positionTracking;
}

FileManager *MarbleModel::fileManager()
{
    return &d->m_fileManager;
}

qreal MarbleModel::planetRadius()   const
{
    return d->m_planet.radius();
}

QString MarbleModel::planetName()   const
{
    return d->m_planet.name();
}

QString MarbleModel::planetId() const
{
    return d->m_planet.id();
}

MarbleClock *MarbleModel::clock()
{
    return &d->m_clock;
}

const MarbleClock *MarbleModel::clock() const
{
    return &d->m_clock;
}

SunLocator *MarbleModel::sunLocator()
{
    return &d->m_sunLocator;
}

const SunLocator *MarbleModel::sunLocator() const
{
    return &d->m_sunLocator;
}

quint64 MarbleModel::persistentTileCacheLimit() const
{
    return d->m_storageWatcher.cacheLimit() / 1024;
}

void MarbleModel::clearPersistentTileCache()
{
    d->m_storagePolicy.clearCache();

    // Now create base tiles again if needed
    if ( d->m_mapTheme->map()->hasTextureLayers() || d->m_mapTheme->map()->hasVectorLayers() ) {
        // If the tiles aren't already there, put up a progress dialog
        // while creating them.

        // As long as we don't have an Layer Management Class we just lookup
        // the name of the layer that has the same name as the theme ID
        QString themeID = d->m_mapTheme->head()->theme();

        const GeoSceneLayer *layer =
            static_cast<const GeoSceneLayer*>( d->m_mapTheme->map()->layer( themeID ) );
        const GeoSceneTileDataset *texture =
            static_cast<const GeoSceneTileDataset*>( layer->groundDataset() );

        QString sourceDir = texture->sourceDir();
        QString installMap = texture->installMap();
        QString role = d->m_mapTheme->map()->layer( themeID )->role();

        if ( !TileLoader::baseTilesAvailable( *texture )
            && !installMap.isEmpty() )
        {
            mDebug() << "Base tiles not available. Creating Tiles ... \n"
                     << "SourceDir: " << sourceDir << "InstallMap:" << installMap;
            MarbleDirs::debug();

            TileCreator *tileCreator = new TileCreator(
                                     sourceDir,
                                     installMap,
                                     (role == QLatin1String("dem")) ? "true" : "false" );
            tileCreator->setTileFormat( texture->fileFormat().toLower() );

            QPointer<TileCreatorDialog> tileCreatorDlg = new TileCreatorDialog( tileCreator, 0 );
            tileCreatorDlg->setSummary( d->m_mapTheme->head()->name(),
                                        d->m_mapTheme->head()->description() );
            tileCreatorDlg->exec();
            qDebug("Tile creation completed");
            delete tileCreatorDlg;
        }
    }
}

void MarbleModel::setPersistentTileCacheLimit(quint64 kiloBytes)
{
    d->m_storageWatcher.setCacheLimit( kiloBytes * 1024 );

    if( kiloBytes != 0 )
    {
        if( !d->m_storageWatcher.isRunning() )
            d->m_storageWatcher.start( QThread::IdlePriority );
    }
    else
    {
        d->m_storageWatcher.quit();
    }
    // TODO: trigger update
}

void MarbleModel::setTrackedPlacemark( const GeoDataPlacemark *placemark )
{
    d->m_trackedPlacemark = placemark;
    emit trackedPlacemarkChanged( placemark );
}

const GeoDataPlacemark* MarbleModel::trackedPlacemark() const
{
    return d->m_trackedPlacemark;
}

const PluginManager* MarbleModel::pluginManager() const
{
    return &d->m_pluginManager;
}

PluginManager* MarbleModel::pluginManager()
{
    return &d->m_pluginManager;
}

const Planet *MarbleModel::planet() const
{
    return &d->m_planet;
}

void MarbleModel::addDownloadPolicies( const GeoSceneDocument *mapTheme )
{
    if ( !mapTheme )
        return;
    if ( !mapTheme->map()->hasTextureLayers() && !mapTheme->map()->hasVectorLayers() )
        return;

    // As long as we don't have an Layer Management Class we just lookup
    // the name of the layer that has the same name as the theme ID
    const QString themeId = mapTheme->head()->theme();
    const GeoSceneLayer * const layer = static_cast<const GeoSceneLayer*>( mapTheme->map()->layer( themeId ));
    if ( !layer )
        return;

    const GeoSceneTileDataset * const texture = static_cast<const GeoSceneTileDataset*>( layer->groundDataset() );
    if ( !texture )
        return;

    QList<const DownloadPolicy *> policies = texture->downloadPolicies();
    QList<const DownloadPolicy *>::const_iterator pos = policies.constBegin();
    QList<const DownloadPolicy *>::const_iterator const end = policies.constEnd();
    for (; pos != end; ++pos ) {
        d->m_downloadManager.addDownloadPolicy( **pos );
    }
}

RoutingManager* MarbleModel::routingManager()
{
    return d->m_routingManager;
}

const RoutingManager* MarbleModel::routingManager() const
{
    return d->m_routingManager;
}

void MarbleModel::setClockDateTime( const QDateTime& datetime )
{
    d->m_clock.setDateTime( datetime );
}

QDateTime MarbleModel::clockDateTime() const
{
    return d->m_clock.dateTime();
}

int MarbleModel::clockSpeed() const
{
    return d->m_clock.speed();
}

void MarbleModel::setClockSpeed( int speed )
{
    d->m_clock.setSpeed( speed );
}

void MarbleModel::setClockTimezone( int timeInSec )
{
    d->m_clock.setTimezone( timeInSec );
}

int MarbleModel::clockTimezone() const
{
    return d->m_clock.timezone();
}

QTextDocument * MarbleModel::legend()
{
    return d->m_legend;
}

void MarbleModel::setLegend( QTextDocument * legend )
{
    delete d->m_legend;
    d->m_legend = legend;
}

void MarbleModel::addGeoDataFile( const QString& filename )
{
    d->m_fileManager.addFile( filename, filename, GeoDataStyle::Ptr(), UserDocument, true );
}

void MarbleModel::addGeoDataString( const QString& data, const QString& key )
{
    d->m_fileManager.addData( key, data, UserDocument );
}

void MarbleModel::removeGeoData( const QString& fileName )
{
    d->m_fileManager.removeFile( fileName );
}

void MarbleModel::updateProperty( const QString &property, bool value )
{
    foreach( GeoDataFeature *feature, d->m_treeModel.rootDocument()->featureList()) {
        if( feature->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataDocument *document = static_cast<GeoDataDocument*>( feature );
            if( document->property() == property ){
                document->setVisible( value );
                d->m_treeModel.updateFeature( document );
            }
        }
    }
}

void MarbleModelPrivate::assignFillColors( const QString &filePath ) {
    foreach( GeoSceneLayer *layer, m_mapTheme->map()->layers() ) {
        if ( layer->backend() == dgml::dgmlValue_geodata 
             || layer->backend() == dgml::dgmlValue_vector )
        {
            foreach( GeoSceneAbstractDataset *dataset, layer->datasets() ) {
                GeoSceneGeodata *data = static_cast<GeoSceneGeodata*>( dataset );
                if ( data ) {
                    if ( data->sourceFile() == filePath ) {
                        GeoDataDocument *doc = m_fileManager.at( filePath );
                        Q_ASSERT( doc );

                        addHighlightStyle( doc );

                        QPen pen = data->pen();
                        QBrush brush = data->brush();
                        const QVector<QColor> colors = data->colors();
                        GeoDataLineStyle lineStyle( pen.color() );
                        lineStyle.setPenStyle( pen.style() );
                        lineStyle.setWidth( pen.width() );

                        if ( !colors.isEmpty() ) {
                            qreal alpha = data->alpha();
                            QVector<GeoDataFeature*>::iterator it = doc->begin();
                            QVector<GeoDataFeature*>::iterator const itEnd = doc->end();
                            for ( ; it != itEnd; ++it ) {
                                GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( *it );
                                if ( placemark ) {
                                    GeoDataStyle::Ptr style(new GeoDataStyle);
                                    style->setId(QStringLiteral("normal"));
                                    style->setLineStyle( lineStyle );
                                    quint8 colorIndex = placemark->style()->polyStyle().colorIndex();
                                    GeoDataPolyStyle polyStyle;
                                    // Set the colorIndex so that it's not lost after setting new style.
                                    polyStyle.setColorIndex( colorIndex );
                                    QColor color;
                                    // color index having value 99 is undefined
                                    Q_ASSERT( colors.size() );
                                    if ( colorIndex > colors.size() || ( colorIndex - 1 ) < 0 )
                                    {
                                        color = colors[0];      // Assign the first color as default
                                    }
                                    else {
                                        color = colors[colorIndex-1];
                                    }
                                    color.setAlphaF( alpha );
                                    polyStyle.setColor( color );
                                    polyStyle.setFill( true );
                                    style->setPolyStyle( polyStyle );
                                    placemark->setStyle( style );
                                }
                            }
                        }
                        else {
                            GeoDataStyle::Ptr style(new GeoDataStyle);
                            GeoDataPolyStyle polyStyle( brush.color() );
                            polyStyle.setFill( true );
                            style->setLineStyle( lineStyle );
                            style->setPolyStyle( polyStyle );
                            style->setId(QStringLiteral("default"));
                            GeoDataStyleMap styleMap;
                            styleMap.setId(QStringLiteral("default-map"));
                            styleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + style->id());
                            doc->addStyle( style );
                            doc->addStyleMap( styleMap );

                            const QString styleUrl = QLatin1Char('#') + styleMap.id();
                            QVector<GeoDataFeature*>::iterator iter = doc->begin();
                            QVector<GeoDataFeature*>::iterator const end = doc->end();

                            for ( ; iter != end; ++iter ) {
                                if ( (*iter)->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
                                    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( *iter );
                                    Q_ASSERT( placemark );
                                    if ( placemark->geometry()->nodeType() != GeoDataTypes::GeoDataTrackType &&
                                        placemark->geometry()->nodeType() != GeoDataTypes::GeoDataPointType )
                                    {
                                        placemark->setStyleUrl(styleUrl);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool MarbleModel::workOffline() const
{
    return d->m_workOffline;
}

void MarbleModel::setWorkOffline( bool workOffline )
{
    if ( d->m_workOffline != workOffline ) {
        downloadManager()->setDownloadEnabled( !workOffline );

        d->m_workOffline = workOffline;
        emit workOfflineChanged();
    }
}

ElevationModel* MarbleModel::elevationModel()
{
    return &d->m_elevationModel;
}

const ElevationModel* MarbleModel::elevationModel() const
{
    return &d->m_elevationModel;
}

}

#include "moc_MarbleModel.cpp"
