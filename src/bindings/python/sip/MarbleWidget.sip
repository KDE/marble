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

class MarbleWidget : QWidget
{
%TypeHeaderCode
#include <MarbleWidget.h>
%End

public:
    explicit                MarbleWidget (QWidget* parent /TransferThis/ = 0);
    explicit                MarbleWidget (Marble::MarbleMap* map, QWidget* parent /TransferThis/ = 0);
    Marble::MarbleMap*      map () const;
    Marble::MarbleModel*    model () const;

// MarbleWidgetInputHandler isn't an exported class
//ig    void                    setInputHandler (Marble::MarbleWidgetInputHandler* handler);

//mt undefined symbol: HttpDownloadManager* downloadManager -- need mapped type?
//mt     void               setDownloadManager (HttpDownloadManager* downloadManager);


    const QRegion           activeRegion ();
    const QRegion           projectedRegion ();
    int                     radius () const;
    void                    setRadius (int radius);
    int                     zoom () const;
    qreal                   distance () const;
    void                    setDistance (qreal distance);
    QString                 distanceString () const;
    int                     minimumZoom () const;
    int                     maximumZoom () const;
    bool                    needsUpdate () const;
    void                    setNeedsUpdate ();
    bool                    screenCoordinates (qreal lon, qreal lat, int& x /out/, int& y /out/);
    int                     northPoleY ();
    bool                    geoCoordinates (int x, int y, qreal& lon /out/, qreal& lat /out/, Marble::GeoDataCoordinates::Unit = Marble::GeoDataCoordinates::Degree);
    bool                    globalQuaternion (int x, int y, Marble::Quaternion& q);
    qreal                   centerLongitude () const;
    qreal                   centerLatitude () const;
    QAbstractItemModel*     placemarkModel () const;
    QItemSelectionModel*    placemarkSelectionModel () const;
    qreal                   moveStep ();
    void                    addPlacemarkFile (const QString& filename);
    void                    addPlacemarkData (const QString& data, const QString& key = "data");
    void                    removePlacemarkKey( const QString& key );
    Marble::Quaternion      planetAxis () const;
    QPixmap                 mapScreenShot ();
    bool                    showOverviewMap () const;
    bool                    showScaleBar () const;
    bool                    showCompass () const;
    bool                    showClouds () const;
    bool                    showAtmosphere () const;
    bool                    showCrosshairs () const;
    bool                    showGrid () const;
    bool                    showPlaces () const;
    bool                    showCities () const;
    bool                    showTerrain () const;
    bool                    showOtherPlaces () const;
    bool                    showRelief () const;
    bool                    showElevationModel () const;
    bool                    showIceLayer () const;
    bool                    showBorders () const;
    bool                    showRivers () const;
    bool                    showLakes () const;
    bool                    showGps () const;
    bool                    showFrameRate () const;
    quint64                 persistentTileCacheLimit () const;
    quint64                 volatileTileCacheLimit () const;
    Marble::SunLocator*     sunLocator ();
    void                    setProxy (const QString& proxyHost, const quint16 proxyPort);
    QString                 proxyHost () const;
    quint16                 proxyPort () const;
    QList<Marble::MarbleRenderPlugin*>  renderPlugins () const;
    QList<Marble::MarbleAbstractFloatItem*>  floatItems () const;
    Marble::Projection      projection () const;
    QString                 mapThemeId () const;

//mt undefined symbol: GeoSceneDocument* None -- need mapped type?
//mt     Marble::GeoSceneDocument*  mapTheme () const;

//mt undefined symbol: GpxFileModel* None -- need mapped type?
//mt     GpxFileModel*      gpxFileModel ();

//ig   FileViewModel*     fileViewModel () const;

    Marble::MapQuality      mapQuality (Marble::ViewContext = Marble::Still);
    Marble::ViewContext     viewContext () const;
    bool                    animationsEnabled () const;
    Marble::AngleUnit       defaultAngleUnit () const;
    void                    setDefaultAngleUnit (Marble::AngleUnit angleUnit);
    QFont                   defaultFont () const;
    void                    setDefaultFont (const QFont& font);
    void                    updateSun ();
    void                    centerSun ();
    void                    enableInput ();
    void                    disableInput ();
    void                    zoomView (int zoom);
    void                    zoomViewBy (int zoomStep);
    void                    zoomIn ();
    void                    zoomOut ();
    void                    rotateBy (const qreal& deltaLon /in/, const qreal& deltaLat /in/);
    void                    rotateBy (const Marble::Quaternion& incRot);
    void                    centerOn (const qreal& lon /in/, const qreal& lat /in/, bool animated = 0);
    void                    centerOn (const QModelIndex& index, bool animated = 0);
    void                    centerOn (const Marble::GeoDataCoordinates& point, bool animated = 0);
    void                    setCenterLatitude (qreal lat);
    void                    setCenterLongitude (qreal lon);
    void                    setProjection (int projection);
    void                    setProjection (Marble::Projection projection /Constrained/);
    void                    home (qreal& lon /out/, qreal& lat /out/, int& zoom /out/);
    void                    setHome (qreal lon, qreal lat, int zoom = 1050);
    void                    setHome (const Marble::GeoDataCoordinates& homePoint, int zoom = 1050);
    void                    moveLeft ();
    void                    moveRight ();
    void                    moveUp ();
    void                    moveDown ();
    void                    goHome ();
    void                    setMapThemeId (const QString& maptheme);
    void                    setPropertyValue (const QString& name, bool value);
    void                    setShowOverviewMap (bool visible);
    void                    setShowScaleBar (bool visible);
    void                    setShowCompass (bool visible);
    void                    setShowClouds (bool visible);
    void                    setShowAtmosphere (bool visible);
    void                    setShowCrosshairs (bool visible);
    void                    setShowGrid (bool visible);
    void                    setShowPlaces (bool visible);
    void                    setShowCities (bool visible);
    void                    setShowTerrain (bool visible);
    void                    setShowOtherPlaces (bool visible);
    void                    setShowRelief (bool visible);
    void                    setShowElevationModel (bool visible);
    void                    setShowIceLayer (bool visible);
    void                    setShowBorders (bool visible);
    void                    setShowRivers (bool visible);
    void                    setShowLakes (bool visible);
    void                    setShowGps (bool visible);
    void                    setShowFrameRate (bool visible);
    void                    changeCurrentPosition (qreal lon, qreal lat);
    void                    notifyMouseClick (int x, int y);
    void                    updateGps ();
    void                    openGpxFile (QString& filename);
    void                    clearPersistentTileCache ();
    void                    setPersistentTileCacheLimit (quint64 kiloBytes);
    void                    clearVolatileTileCache ();
    void                    setVolatileTileCacheLimit (quint64 kiloBytes);
    void                    creatingTilesStart (Marble::TileCreator* creator, const QString& name, const QString& description);
    void                    updateChangedMap ();

//    void                    updateRegion (Marble::BoundingBox&);
    void                    setDownloadUrl (const QString& url);
    void                    setDownloadUrl (const QUrl& url);
    void                    setMapQuality (Marble::MapQuality, Marble::ViewContext = Marble::Still);
    void                    setViewContext (Marble::ViewContext viewContext);
    void                    setAnimationsEnabled (bool enabled);
    void                    setSelection (const QRect& region);

signals:
    void                    zoomChanged (int zoom);
    void                    distanceChanged (const QString& distanceString);
    void                    themeChanged (QString theme);
    void                    projectionChanged (Marble::Projection);
    void                    mouseMoveGeoPosition (QString);
    void                    mouseClickGeoPosition (qreal lon, qreal lat, Marble::GeoDataCoordinates::Unit);
    void                    timeout ();
    void                    framesPerSecond (qreal fps);

//mt undefined symbol: QList<double>& None -- need mapped type?
//mt     void               regionSelected (const QList<double>&);

protected:
    void                    leaveEvent (QEvent* event);
    void                    paintEvent (QPaintEvent* event);
    virtual void            customPaint (Marble::GeoPainter* painter);
    void                    resizeEvent (QResizeEvent*);
    void                    connectNotify (const char* signal);
    void                    disconnectNotify (const char* signal);

private:
                            MarbleWidget (const Marble::MarbleWidget&);
};   // MarbleWidget

};   // Marble


