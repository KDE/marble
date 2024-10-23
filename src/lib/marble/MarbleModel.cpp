// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>
// SPDX-FileCopyrightText: 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "MarbleModel.h"

#include <cmath>

#include <QAbstractItemModel>
#include <QAtomicInt>
#include <QItemSelectionModel>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QTextDocument>

#include "kdescendantsproxymodel.h"

#include "MapThemeManager.h"
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
#include "GeoDataLineStyle.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"

#include "BookmarkManager.h"
#include "DgmlAuxillaryDictionary.h"
#include "ElevationModel.h"
#include "FileManager.h"
#include "FileStoragePolicy.h"
#include "FileStorageWatcher.h"
#include "GeoDataTreeModel.h"
#include "HttpDownloadManager.h"
#include "MarbleClock.h"
#include "MarbleDirs.h"
#include "PlacemarkPositionProviderPlugin.h"
#include "Planet.h"
#include "PlanetFactory.h"
#include "PluginManager.h"
#include "PositionTracking.h"
#include "RouteSimulationPositionProviderPlugin.h"
#include "StoragePolicy.h"
#include "SunLocator.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"
#include "routing/RoutingManager.h"

namespace Marble
{

class MarbleModelPrivate
{
public:
    MarbleModelPrivate()
        : m_clock()
        , m_planet(PlanetFactory::construct(QStringLiteral("earth")))
        , m_sunLocator(&m_clock, &m_planet)
        , m_pluginManager()
        , m_homePoint(-9.4, 54.8, 0.0, GeoDataCoordinates::Degree)
        , // Some point that tackat defined. :-)
        m_homeZoom(1050)
        , m_mapTheme(nullptr)
        , m_storagePolicy(MarbleDirs::localPath())
        , m_downloadManager(&m_storagePolicy)
        , m_storageWatcher(MarbleDirs::localPath())
        , m_treeModel()
        , m_descendantProxy()
        , m_placemarkProxyModel()
        , m_placemarkSelectionModel(nullptr)
        , m_fileManager(&m_treeModel, &m_pluginManager)
        , m_positionTracking(&m_treeModel)
        , m_trackedPlacemark(nullptr)
        , m_bookmarkManager(&m_treeModel)
        , m_routingManager(nullptr)
        , m_legend(nullptr)
        , m_workOffline(false)
        , m_elevationModel(&m_downloadManager, &m_pluginManager)
    {
        m_descendantProxy.setSourceModel(&m_treeModel);

        m_placemarkProxyModel.setFilterFixedString(QString::fromLatin1(GeoDataTypes::GeoDataPlacemarkType));
        m_placemarkProxyModel.setFilterKeyColumn(1);
        m_placemarkProxyModel.setSourceModel(&m_descendantProxy);
        m_placemarkSelectionModel.setModel(&m_placemarkProxyModel);

        m_groundOverlayProxyModel.setFilterFixedString(QString::fromLatin1(GeoDataTypes::GeoDataGroundOverlayType));
        m_groundOverlayProxyModel.setFilterKeyColumn(1);
        m_groundOverlayProxyModel.setSourceModel(&m_descendantProxy);
    }

    ~MarbleModelPrivate()
    {
        delete m_mapTheme;
        delete m_legend;
    }
    /**
     * @brief Assigns each placemark an inline
     * style based on the color values specified
     * by colorMap attribute under <brush> element
     * in theme file.
     */
    void assignFillColors(const QString &filePath);
    void assignFillColors(GeoDataDocument *doc, const GeoSceneGeodata &data) const;

    void addHighlightStyle(GeoDataDocument *doc) const;

    // Misc stuff.
    MarbleClock m_clock;
    Planet m_planet;
    SunLocator m_sunLocator;

    PluginManager m_pluginManager;

    // The home position
    GeoDataCoordinates m_homePoint;
    int m_homeZoom;

    // View and paint stuff
    GeoSceneDocument *m_mapTheme;

    FileStoragePolicy m_storagePolicy;
    HttpDownloadManager m_downloadManager;

    // Cache related
    FileStorageWatcher m_storageWatcher;

    // Places on the map
    GeoDataTreeModel m_treeModel;
    KDescendantsProxyModel m_descendantProxy;
    QSortFilterProxyModel m_placemarkProxyModel;
    QSortFilterProxyModel m_groundOverlayProxyModel;

    // Selection handling
    QItemSelectionModel m_placemarkSelectionModel;

    FileManager m_fileManager;

    // Gps Stuff
    PositionTracking m_positionTracking;

    const GeoDataPlacemark *m_trackedPlacemark;

    BookmarkManager m_bookmarkManager;
    RoutingManager *m_routingManager;
    QTextDocument *m_legend;

    bool m_workOffline;

    ElevationModel m_elevationModel;
};

MarbleModel::MarbleModel(QObject *parent)
    : QObject(parent)
    , d(new MarbleModelPrivate())
{
    // connect the StoragePolicy used by the download manager to the FileStorageWatcher
    connect(&d->m_storagePolicy, SIGNAL(cleared()), &d->m_storageWatcher, SLOT(resetCurrentSize()));
    connect(&d->m_storagePolicy, SIGNAL(sizeChanged(qint64)), &d->m_storageWatcher, SLOT(addToCurrentSize(qint64)));

    connect(&d->m_fileManager, SIGNAL(fileAdded(QString)), this, SLOT(assignFillColors(QString)));

    d->m_routingManager = new RoutingManager(this, this);

    connect(&d->m_clock, SIGNAL(timeChanged()), &d->m_sunLocator, SLOT(update()));

    d->m_pluginManager.addPositionProviderPlugin(new PlacemarkPositionProviderPlugin(this, this));
    d->m_pluginManager.addPositionProviderPlugin(new RouteSimulationPositionProviderPlugin(this, this));
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

void MarbleModel::setMapThemeId(const QString &mapThemeId)
{
    if (!mapThemeId.isEmpty() && mapThemeId == this->mapThemeId())
        return;

    GeoSceneDocument *mapTheme = MapThemeManager::loadMapTheme(mapThemeId);
    setMapTheme(mapTheme);
}

void MarbleModel::setMapTheme(GeoSceneDocument *document)
{
    GeoSceneDocument *mapTheme = document;
    if (!mapTheme) {
        // Check whether the previous theme works
        if (d->m_mapTheme) {
            qWarning() << "Selected theme doesn't work, so we stick to the previous one";
            return;
        }

        // Fall back to default theme
        QString defaultTheme = QStringLiteral("earth/srtm/srtm.dgml");
        qWarning() << "Falling back to default theme:" << defaultTheme;
        mapTheme = MapThemeManager::loadMapTheme(defaultTheme);
    }

    // If this last resort doesn't work either shed a tear and exit
    if (!mapTheme) {
        qWarning() << "Couldn't find a valid DGML map.";
        return;
    }

    // find the list of previous theme's geodata
    QList<GeoSceneGeodata> currentDatasets;
    if (d->m_mapTheme) {
        for (GeoSceneLayer *layer : d->m_mapTheme->map()->layers()) {
            if (layer->backend() != QString::fromLatin1(dgml::dgmlValue_geodata) && layer->backend() != QString::fromLatin1(dgml::dgmlValue_vector))
                continue;

            // look for documents
            for (GeoSceneAbstractDataset *dataset : layer->datasets()) {
                auto data = dynamic_cast<GeoSceneGeodata *>(dataset);
                Q_ASSERT(data);
                currentDatasets << *data;
            }
        }
    }

    delete d->m_mapTheme;
    d->m_mapTheme = mapTheme;

    addDownloadPolicies(d->m_mapTheme);

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
    // Don't change the planet unless we have to...
    qreal const radiusAttributeValue = d->m_mapTheme->head()->radius();
    if (d->m_mapTheme->head()->target().toLower() != d->m_planet.id() || radiusAttributeValue != d->m_planet.radius()) {
        mDebug() << "Changing Planet";
        d->m_planet = PlanetFactory::construct(d->m_mapTheme->head()->target().toLower());
        if (radiusAttributeValue > 0.0) {
            d->m_planet.setRadius(radiusAttributeValue);
        }
        sunLocator()->setPlanet(&d->m_planet);
    }

    QStringList fileList;
    QStringList propertyList;
    QList<GeoDataStyle::Ptr> styleList;
    QList<int> renderOrderList;

    for (GeoSceneLayer *layer : d->m_mapTheme->map()->layers()) {
        if (layer->backend() != QString::fromLatin1(dgml::dgmlValue_geodata) && layer->backend() != QString::fromLatin1(dgml::dgmlValue_vector))
            continue;

        // look for datasets which are different from currentDatasets
        for (const GeoSceneAbstractDataset *dataset : layer->datasets()) {
            const auto data = dynamic_cast<const GeoSceneGeodata *>(dataset);
            Q_ASSERT(data);
            bool skip = false;
            GeoDataDocument *doc = nullptr;
            for (int i = 0; i < currentDatasets.size(); ++i) {
                if (currentDatasets[i] == *data) {
                    currentDatasets.removeAt(i);
                    skip = true;
                    break;
                }
                /*
                 * If the sourcefile of data matches any in the currentDatasets then there
                 * is no need to parse the file again just update the style
                 * i.e. <brush> and <pen> values of already parsed file. assignNewStyle() does that
                 */
                if (currentDatasets[i].sourceFile() == data->sourceFile()) {
                    doc = d->m_fileManager.at(data->sourceFile());
                    currentDatasets.removeAt(i);
                }
            }
            if (skip) {
                continue;
            }

            if (doc) {
                d->assignFillColors(doc, *data);
            } else {
                const QString filename = data->sourceFile();
                const QString property = data->property();
                const QPen pen = data->pen();
                const QBrush brush = data->brush();
                GeoDataStyle::Ptr style;
                const int renderOrder = data->renderOrder();

                /*
                 * data->colors() are the colorMap values from dgml file. If this is not
                 * empty then we are supposed to assign every placemark a different style
                 * by giving it a color from colorMap values based on color index
                 * of that placemark. See assignFillColors() for details. So, we need to
                 * send an empty style to fileManeger otherwise the FileLoader::createFilterProperties()
                 * will overwrite the parsed value of color index ( GeoDataPolyStyle::d->m_colorIndex ).
                 */
                if (data->colors().isEmpty()) {
                    GeoDataLineStyle lineStyle(pen.color());
                    lineStyle.setPenStyle(pen.style());
                    lineStyle.setWidth(pen.width());
                    GeoDataPolyStyle polyStyle(brush.color());
                    polyStyle.setFill(true);
                    style = GeoDataStyle::Ptr(new GeoDataStyle);
                    style->setLineStyle(lineStyle);
                    style->setPolyStyle(polyStyle);
                    style->setId(QStringLiteral("default"));
                }

                fileList << filename;
                propertyList << property;
                styleList << style;
                renderOrderList << renderOrder;
            }
        }
    }
    // unload old currentDatasets which are not part of the new map
    for (const GeoSceneGeodata &data : std::as_const(currentDatasets)) {
        d->m_fileManager.removeFile(data.sourceFile());
    }
    // load new datasets
    for (int i = 0; i < fileList.size(); ++i) {
        d->m_fileManager.addFile(fileList.at(i), propertyList.at(i), styleList.at(i), MapDocument, renderOrderList.at(i));
    }

    mDebug() << "THEME CHANGED: ***" << mapTheme->head()->mapThemeId();
    Q_EMIT themeChanged(mapTheme->head()->mapThemeId());
}

void MarbleModelPrivate::addHighlightStyle(GeoDataDocument *doc) const
{
    if (doc) {
        /*
         * Add a highlight style to GeoDataDocument if
         *the theme file specifies any highlight color.
         */
        QColor highlightBrushColor = m_mapTheme->map()->highlightBrushColor();
        QColor highlightPenColor = m_mapTheme->map()->highlightPenColor();

        GeoDataStyle::Ptr highlightStyle(new GeoDataStyle);
        highlightStyle->setId(QStringLiteral("highlight"));

        if (highlightBrushColor.isValid()) {
            GeoDataPolyStyle highlightPolyStyle;
            highlightPolyStyle.setColor(highlightBrushColor);
            highlightPolyStyle.setFill(true);
            highlightStyle->setPolyStyle(highlightPolyStyle);
        }
        if (highlightPenColor.isValid()) {
            GeoDataLineStyle highlightLineStyle(highlightPenColor);
            highlightStyle->setLineStyle(highlightLineStyle);
        }
        if (highlightBrushColor.isValid() || highlightPenColor.isValid()) {
            GeoDataStyleMap styleMap = doc->styleMap(QStringLiteral("default-map"));
            styleMap.insert(QStringLiteral("highlight"), QLatin1Char('#') + highlightStyle->id());
            doc->addStyle(highlightStyle);
            doc->addStyleMap(styleMap);
        }
    }
}

void MarbleModel::home(qreal &lon, qreal &lat, int &zoom) const
{
    d->m_homePoint.geoCoordinates(lon, lat, GeoDataCoordinates::Degree);
    zoom = d->m_homeZoom;
}

void MarbleModel::setHome(qreal lon, qreal lat, int zoom)
{
    d->m_homePoint = GeoDataCoordinates(lon, lat, 0, GeoDataCoordinates::Degree);
    d->m_homeZoom = zoom;
    Q_EMIT homeChanged(d->m_homePoint);
}

void MarbleModel::setHome(const GeoDataCoordinates &homePoint, int zoom)
{
    d->m_homePoint = homePoint;
    d->m_homeZoom = zoom;
    Q_EMIT homeChanged(d->m_homePoint);
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

qreal MarbleModel::planetRadius() const
{
    return d->m_planet.radius();
}

QString MarbleModel::planetName() const
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
    if (d->m_mapTheme->map()->hasTextureLayers() || d->m_mapTheme->map()->hasVectorLayers()) {
        // If the tiles aren't already there, put up a progress dialog
        // while creating them.

        // As long as we don't have an Layer Management Class we just lookup
        // the name of the layer that has the same name as the theme ID
        QString themeID = d->m_mapTheme->head()->theme();

        const auto layer = static_cast<const GeoSceneLayer *>(d->m_mapTheme->map()->layer(themeID));
        const auto texture = static_cast<const GeoSceneTileDataset *>(layer->groundDataset());

        QString sourceDir = texture->sourceDir();
        QString installMap = texture->installMap();
        QString role = d->m_mapTheme->map()->layer(themeID)->role();

        if (!TileLoader::baseTilesAvailable(*texture) && !installMap.isEmpty()) {
            mDebug() << "Base tiles not available. Creating Tiles ... \n"
                     << "SourceDir: " << sourceDir << "InstallMap:" << installMap;
            MarbleDirs::debug();

            auto tileCreator = new TileCreator(sourceDir, installMap, (role == QLatin1StringView("dem")) ? QStringLiteral("true") : QStringLiteral("false"));
            tileCreator->setTileFormat(texture->fileFormat().toLower());

            QPointer<TileCreatorDialog> tileCreatorDlg = new TileCreatorDialog(tileCreator, nullptr);
            tileCreatorDlg->setSummary(d->m_mapTheme->head()->name(), d->m_mapTheme->head()->description());
            tileCreatorDlg->exec();
            qDebug("Tile creation completed");
            delete tileCreatorDlg;
        }
    }
}

void MarbleModel::setPersistentTileCacheLimit(quint64 kiloBytes)
{
    d->m_storageWatcher.setCacheLimit(kiloBytes * 1024);

    if (kiloBytes != 0) {
        if (!d->m_storageWatcher.isRunning())
            d->m_storageWatcher.start(QThread::IdlePriority);
    } else {
        d->m_storageWatcher.quit();
    }
    // TODO: trigger update
}

void MarbleModel::setTrackedPlacemark(const GeoDataPlacemark *placemark)
{
    d->m_trackedPlacemark = placemark;
    Q_EMIT trackedPlacemarkChanged(placemark);
}

const GeoDataPlacemark *MarbleModel::trackedPlacemark() const
{
    return d->m_trackedPlacemark;
}

const PluginManager *MarbleModel::pluginManager() const
{
    return &d->m_pluginManager;
}

PluginManager *MarbleModel::pluginManager()
{
    return &d->m_pluginManager;
}

const Planet *MarbleModel::planet() const
{
    return &d->m_planet;
}

void MarbleModel::addDownloadPolicies(const GeoSceneDocument *mapTheme)
{
    if (!mapTheme)
        return;
    if (!mapTheme->map()->hasTextureLayers() && !mapTheme->map()->hasVectorLayers())
        return;

    // As long as we don't have an Layer Management Class we just lookup
    // the name of the layer that has the same name as the theme ID
    const QString themeId = mapTheme->head()->theme();
    auto const *layer = static_cast<const GeoSceneLayer *>(mapTheme->map()->layer(themeId));
    if (!layer)
        return;

    auto const *texture = static_cast<const GeoSceneTileDataset *>(layer->groundDataset());
    if (!texture)
        return;

    QList<const DownloadPolicy *> policies = texture->downloadPolicies();
    QList<const DownloadPolicy *>::const_iterator pos = policies.constBegin();
    QList<const DownloadPolicy *>::const_iterator const end = policies.constEnd();
    for (; pos != end; ++pos) {
        d->m_downloadManager.addDownloadPolicy(**pos);
    }
}

RoutingManager *MarbleModel::routingManager()
{
    return d->m_routingManager;
}

const RoutingManager *MarbleModel::routingManager() const
{
    return d->m_routingManager;
}

void MarbleModel::setClockDateTime(const QDateTime &datetime)
{
    d->m_clock.setDateTime(datetime);
}

QDateTime MarbleModel::clockDateTime() const
{
    return d->m_clock.dateTime();
}

int MarbleModel::clockSpeed() const
{
    return d->m_clock.speed();
}

void MarbleModel::setClockSpeed(int speed)
{
    d->m_clock.setSpeed(speed);
}

void MarbleModel::setClockTimezone(int timeInSec)
{
    d->m_clock.setTimezone(timeInSec);
}

int MarbleModel::clockTimezone() const
{
    return d->m_clock.timezone();
}

QTextDocument *MarbleModel::legend()
{
    return d->m_legend;
}

void MarbleModel::setLegend(QTextDocument *legend)
{
    delete d->m_legend;
    d->m_legend = legend;
}

void MarbleModel::addGeoDataFile(const QString &filename)
{
    d->m_fileManager.addFile(filename, filename, GeoDataStyle::Ptr(), UserDocument, 0, true);
}

void MarbleModel::addGeoDataString(const QString &data, const QString &key)
{
    d->m_fileManager.addData(key, data, UserDocument);
}

void MarbleModel::removeGeoData(const QString &fileName)
{
    d->m_fileManager.removeFile(fileName);
}

void MarbleModel::updateProperty(const QString &property, bool value)
{
    for (GeoDataFeature *feature : d->m_treeModel.rootDocument()->featureList()) {
        if (auto document = geodata_cast<GeoDataDocument>(feature)) {
            if (document->property() == property) {
                document->setVisible(value);
                d->m_treeModel.updateFeature(document);
            }
        }
    }
}

void MarbleModelPrivate::assignFillColors(const QString &filePath)
{
    const GeoSceneGeodata *data = nullptr;

    for (auto layer : m_mapTheme->map()->layers()) {
        if (layer->backend() != QString::fromLatin1(dgml::dgmlValue_geodata) && layer->backend() != QString::fromLatin1(dgml::dgmlValue_vector)) {
            continue;
        }

        for (auto dataset : layer->datasets()) {
            auto sceneData = dynamic_cast<const GeoSceneGeodata *>(dataset);
            if (sceneData != nullptr && sceneData->sourceFile() == filePath) {
                data = sceneData;
                break;
            }
        }

        if (data) {
            break;
        }
    }

    if (data == nullptr) {
        return;
    }

    GeoDataDocument *doc = m_fileManager.at(filePath);
    Q_ASSERT(doc);

    assignFillColors(doc, *data);
}

void MarbleModelPrivate::assignFillColors(GeoDataDocument *doc, const GeoSceneGeodata &data) const
{
    addHighlightStyle(doc);

    const QPen pen = data.pen();
    const QBrush brush = data.brush();
    const QList<QColor> colors = data.colors();
    GeoDataLineStyle lineStyle(pen.color());
    lineStyle.setPenStyle(pen.style());
    lineStyle.setWidth(pen.width());

    if (!colors.isEmpty()) {
        const qreal alpha = data.alpha();
        for (auto feature : *doc) {
            auto placemark = geodata_cast<GeoDataPlacemark>(feature);
            if (placemark == nullptr) {
                continue;
            }

            GeoDataStyle::Ptr style(new GeoDataStyle);
            style->setId(QStringLiteral("normal"));
            style->setLineStyle(lineStyle);
            quint8 colorIndex = placemark->style()->polyStyle().colorIndex();
            GeoDataPolyStyle polyStyle;
            // Set the colorIndex so that it's not lost after setting new style.
            polyStyle.setColorIndex(colorIndex);
            QColor color;
            // color index having value 99 is undefined
            Q_ASSERT(colors.size());
            if (colorIndex > colors.size() || (colorIndex - 1) < 0) {
                color = colors[0]; // Assign the first color as default
            } else {
                color = colors[colorIndex - 1];
            }
            color.setAlphaF(alpha);
            polyStyle.setColor(color);
            polyStyle.setFill(true);
            style->setPolyStyle(polyStyle);
            placemark->setStyle(style);
        }
    } else {
        GeoDataStyle::Ptr style(new GeoDataStyle);
        GeoDataPolyStyle polyStyle(brush.color());
        polyStyle.setFill(true);
        style->setLineStyle(lineStyle);
        style->setPolyStyle(polyStyle);
        style->setId(QStringLiteral("default"));
        GeoDataStyleMap styleMap;
        styleMap.setId(QStringLiteral("default-map"));
        styleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + style->id());
        doc->addStyle(style);
        doc->addStyleMap(styleMap);

        const QString styleUrl = QLatin1Char('#') + styleMap.id();

        for (auto feature : *doc) {
            auto placemark = geodata_cast<GeoDataPlacemark>(feature);
            if (placemark == nullptr) {
                continue;
            }

            if (geodata_cast<GeoDataTrack>(placemark->geometry()) || geodata_cast<GeoDataPoint>(placemark->geometry())) {
                continue;
            }

            placemark->setStyleUrl(styleUrl);
        }
    }
}

bool MarbleModel::workOffline() const
{
    return d->m_workOffline;
}

void MarbleModel::setWorkOffline(bool workOffline)
{
    if (d->m_workOffline != workOffline) {
        downloadManager()->setDownloadEnabled(!workOffline);

        d->m_workOffline = workOffline;
        Q_EMIT workOfflineChanged();
    }
}

ElevationModel *MarbleModel::elevationModel()
{
    return &d->m_elevationModel;
}

const ElevationModel *MarbleModel::elevationModel() const
{
    return &d->m_elevationModel;
}

}

#include "moc_MarbleModel.cpp"
