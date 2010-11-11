//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

namespace Marble
{

class MarbleModel : QObject
{
%TypeHeaderCode
#include <MarbleModel.h>
%End

public:
    explicit                MarbleModel (QObject* parent /TransferThis/);
    QAbstractItemModel*     placemarkModel () const;
    QItemSelectionModel*    placemarkSelectionModel () const;
    QString                 mapThemeId () const;
    void                    setupTextureMapper (Marble::Projection projection);
    void                    addPlacemarkFile (const QString& filename);
    void                    addPlacemarkData (const QString& data, const QString& key = "data");
    void                    removePlacemarkKey (const QString& key);
    QVector<QModelIndex>    whichFeatureAt (const QPoint&) const;
//ig    FileViewModel*          fileViewModel () const;
    qreal                   planetRadius () const;
    QString                 planetName () const;
    Marble::SunLocator*     sunLocator () const;
    quint64                 volatileTileCacheLimit () const;
    QList<Marble::RenderPlugin*>  renderPlugins () const;
    QList<Marble::AbstractFloatItem*>  floatItems () const;
    QList<Marble::AbstractDataPlugin*>  dataPlugins () const;
    QList<Marble::AbstractDataPluginItem*>  whichItemAt (const QPoint& curpos) const;
    Marble::Planet*         planet () const;
    int                     tileZoomLevel () const;
    void                    clearVolatileTileCache ();
    void                    setVolatileTileCacheLimit (quint64 kiloBytes);
    void                    clearPersistentTileCache ();
    void                    update ();

signals:
    void                    creatingTilesStart (Marble::TileCreator*, const QString& name, const QString& description);
    void                    themeChanged (QString mapTheme);
    void                    modelChanged ();

public:
    virtual ~MarbleModel ();
//ig    Marble::GeoSceneDocument*  mapTheme () const;
//ig    HttpDownloadManager*    downloadManager () const;
    void                    openGpxFile (const QString& filename);
//ig    PlacemarkLayout*        placemarkLayout () const;
//ig    AbstractScanlineTextureMapper*  textureMapper () const;
    void                    reloadMap () const;
signals:
    void                    pluginSettingsChanged ();
    void                    repaintNeeded (QRegion dirtyRegion = QRegion());
    void                    renderPluginInitialized (Marble::RenderPlugin* renderPlugin);
public:
    void                    paintGlobe (Marble::GeoPainter* painter, Marble::ViewParams* viewParams, bool redrawBackground, const QRect& dirtyRect);
    QAbstractItemModel*     treeModel () const;
//ig    void                    setMapTheme (GeoSceneDocument* mapTheme, Marble::Projection currentProjection);
    void                    home (qreal& lon, qreal& lat, int& zoom);
    void                    setHome (qreal lon, qreal lat, int zoom = 1050);
    void                    setHome (const Marble::GeoDataCoordinates& homePoint, int zoom = 1050);
    void                    addGeoDataFile (const QString& filename);
    void                    addGeoDataString (const QString& data, const QString& key = "data");
    void                    removeGeoData (const QString& key);
    Marble::PositionTracking*  positionTracking () const;
//ig    MarbleClock*            clock () const;
    void                    setShowTileId (bool show);
    quint64                 persistentTileCacheLimit () const;
//ig    PluginManager*          pluginManager () const;
    void                    addLayer (Marble::LayerInterface* layer);
    void                    removeLayer (Marble::LayerInterface* layer);
//ig    MeasureTool*            measureTool ();
    void                    downloadRegion (const QString& mapThemeId, const QVector<Marble::TileCoordsPyramid>&) const;
//ig    Marble::RoutingManager*  routingManager ();
    void                    setClockDateTime (const QDateTime& datetime);
    QDateTime               clockDateTime () const;
    int                     clockSpeed () const;
    void                    setClockSpeed (int speed);
    void                    setClockTimezone (int timeInSec);
    int                     clockTimezone () const;
//ig    BookmarkManager*        bookmarkManager () const;
    QTextDocument*          legend ();
    void                    setLegend (QTextDocument* document);
    bool                    backgroundVisible () const;
    void                    setBackgroundVisible (bool visible);
    void                    setPersistentTileCacheLimit (quint64 kiloBytes);
//ig    void                    paintTile (StackedTile* tile, const GeoSceneTexture* textureLayer);
signals:
    void                    tileLevelChanged (int newTileLevel);
};
// MarbleModel

};
// Marble


