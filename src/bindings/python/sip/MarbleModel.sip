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
    QAbstractItemModel*     geometryModel () const;
    QString                 mapThemeId () const;


//mt undefined symbol: GeoSceneDocument* None -- need mapped type?
//mt     Marble::GeoSceneDocument*  mapTheme () const;




//mt undefined symbol: GeoSceneDocument* mapTheme -- need mapped type?
//mt     void               setMapTheme (Marble::GeoSceneDocument* mapTheme, Marble::Projection currentProjection);


    void                    setupTextureMapper (Marble::Projection projection);


//mt undefined symbol: HttpDownloadManager* None -- need mapped type?
//mt     HttpDownloadManager*  downloadManager () const;




//mt undefined symbol: HttpDownloadManager* downloadManager -- need mapped type?
//mt     void               setDownloadManager (HttpDownloadManager* downloadManager);


    void                    addPlacemarkFile (const QString& filename);
    void                    addPlacemarkData (const QString& data, const QString& key = "data");
    void                    removePlacemarkKey( const QString& key );
    QVector<QPersistentModelIndex>  whichFeatureAt (const QPoint&) const;

//mt undefined symbol: PlacemarkLayout* None -- need mapped type?
//mt     PlacemarkLayout*   placemarkLayout () const;

//mt undefined symbol: VectorComposer* None -- need mapped type?
//mt     VectorComposer*    vectorComposer () const;

//mt undefined symbol: TextureColorizer* None -- need mapped type?
//mt     TextureColorizer*  textureColorizer () const;

//mt undefined symbol: AbstractScanlineTextureMapper* None -- need mapped type?
//mt     AbstractScanlineTextureMapper*  textureMapper () const;

//mt undefined symbol: GpsLayer* None -- need mapped type?
//mt     GpsLayer*          gpsLayer () const;

//mt undefined symbol: GpxFileModel* None -- need mapped type?
//mt     GpxFileModel*      gpxFileModel () const;

//ig     FileViewModel*     fileViewModel () const;

    bool                    showGps () const;
    void                    setShowGps (bool visible);
    void                    startPolling (int time = 1000);
    void                    stopPolling ();
    qreal                   planetRadius () const;
//    Marble::ExtDateTime*    dateTime () const;
    Marble::SunLocator*     sunLocator () const;

//mt undefined symbol: MergedLayerDecorator* None -- need mapped type?
//mt     MergedLayerDecorator*  layerDecorator () const;

    quint64                 volatileTileCacheLimit () const;
    QList<Marble::MarbleRenderPlugin*>  renderPlugins () const;
    QList<Marble::MarbleAbstractFloatItem*>  floatItems () const;
    void                    clearVolatileTileCache ();
    void                    setVolatileTileCacheLimit (quint64 kiloBytes);
    void                    clearPersistentTileCache ();

//ig     void               paintTile (TextureTile* tile, int x, int y, int level, Marble::GeoSceneTexture* textureLayer, bool requestTileUpdate);

    void                    update ();

signals:
    void                    creatingTilesStart (Marble::TileCreator*, const QString& name, const QString& description);
    void                    themeChanged (QString mapTheme);
    void                    modelChanged ();
//    void                    regionChanged (Marble::BoundingBox& rect);
    void                    timeout ();

private:
                            MarbleModel (const Marble::MarbleModel&);
};   // MarbleModel

};   // Marble


