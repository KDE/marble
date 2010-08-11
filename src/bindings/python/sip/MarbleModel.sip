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
    void                    paintGlobe (Marble::GeoPainter* painter, int width, int height, Marble::ViewParams* viewParams, bool redrawBackground, const QRect& dirtyRect);
    QAbstractItemModel*     placemarkModel () const;
    QItemSelectionModel*    placemarkSelectionModel () const;
    QString                 mapThemeId () const;
    void                    setupTextureMapper (Marble::Projection projection);
    void                    addPlacemarkFile (const QString& filename);
    void                    addPlacemarkData (const QString& data, const QString& key = "data");
    void                    removePlacemarkKey (const QString& key);
    QVector<QModelIndex>    whichFeatureAt (const QPoint&) const;
//ig    FileViewModel*          fileViewModel () const;
    bool                    showGps () const;
    void                    setShowGps (bool visible);
    void                    startPolling (int time = 1000);
    void                    stopPolling ();
    qreal                   planetRadius () const;
    QString                 planetName () const;
//ig    Marble::MarbleClock*    dateTime () const;
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
//ig    void                    paintTile (TextureTile* tile, int x, int y, int level, Marble::GeoSceneTexture* textureLayer, bool requestTileUpdate);
    void                    update ();

signals:
    void                    creatingTilesStart (Marble::TileCreator*, const QString& name, const QString& description);
    void                    themeChanged (QString mapTheme);
    void                    modelChanged ();
    void                    timeout ();

public:
    virtual ~MarbleModel ();
//ig    Marble::GeoSceneDocument*  mapTheme () const;
//ig    void                    setMapTheme (Marble::GeoSceneDocument* mapTheme, Marble::Projection currentProjection);
    void                    setupVectorComposer ();
//ig    HttpDownloadManager*    downloadManager () const;
//ig    void                    setDownloadManager (HttpDownloadManager* downloadManager);
    void                    openGpxFile (const QString& filename);
//ig    PlacemarkLayout*        placemarkLayout () const;
//ig    VectorComposer*         vectorComposer () const;
//ig    TextureColorizer*       textureColorizer () const;
//ig    AbstractScanlineTextureMapper*  textureMapper () const;
//ig    GpsLayer*               gpsLayer () const;
//ig    GpxFileModel*           gpxFileModel () const;
//ig    MergedLayerDecorator*   layerDecorator () const;
    void                    reloadMap () const;
signals:
    void                    pluginSettingsChanged ();
    void                    repaintNeeded (QRegion dirtyRegion = QRegion());
    void                    downloadTile (const QUrl& sourceUrl, const QString& destinationFileName, const QString& id) const;
    void                    renderPluginInitialized (Marble::RenderPlugin* renderPlugin);
};
// MarbleModel

};
// Marble


