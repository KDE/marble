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

class MarbleMap : QObject
{
%TypeHeaderCode
#include <MarbleMap.h>
%End

public:
                            MarbleMap ();
    explicit                MarbleMap (Marble::MarbleModel* model);
    Marble::MarbleModel*    model () const;
    void                    setSize (int width, int height);
    QSize                   size () const;
    int                     width () const;
    int                     height () const;
    int                     radius () const;
    void                    setRadius (int radius);
    int                     minimumZoom () const;
    int                     maximumZoom () const;
    qreal                   centerLongitude () const;
    qreal                   centerLatitude () const;
    bool                    propertyValue (const QString& name) const;
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
    bool                    showIceLayer () const;
    bool                    showBorders () const;
    bool                    showRivers () const;
    bool                    showLakes () const;
    bool                    showFrameRate () const;
    quint64                 volatileTileCacheLimit () const;
    QList<Marble::RenderPlugin*>  renderPlugins () const;
    QList<Marble::AbstractFloatItem*>  floatItems () const;
    void                    rotateBy (const qreal& deltaLon /in/, const qreal& deltaLat /in/);
    void                    setCenterLatitude (qreal lat);
    void                    setCenterLongitude (qreal lon);
    Marble::Projection      projection () const;
    void                    setProjection (Marble::Projection projection);
    QString                 mapThemeId () const;
    void                    setMapThemeId (const QString& maptheme);
    void                    setPropertyValue (const QString& name, bool value);
    void                    setShowOverviewMap (bool visible);
    void                    setShowScaleBar (bool visible);
    void                    setShowCompass (bool visible);
    void                    setShowClouds (bool visible);
    void                    setShowTileId (bool visible);
    void                    setShowAtmosphere (bool visible);
    void                    setShowCrosshairs (bool visible);
    void                    setShowGrid (bool visible);
    void                    setShowPlaces (bool visible);
    void                    setShowCities (bool visible);
    void                    setShowTerrain (bool visible);
    void                    setShowOtherPlaces (bool visible);
    void                    setShowRelief (bool visible);
    void                    setShowIceLayer (bool visible);
    void                    setShowBorders (bool visible);
    void                    setShowRivers (bool visible);
    void                    setShowLakes (bool visible);
    void                    setShowFrameRate (bool visible);
    void                    notifyMouseClick (int x, int y);
    void                    clearVolatileTileCache ();
    void                    setVolatileTileCacheLimit (quint64 kiloBytes);
    Marble::AngleUnit       defaultAngleUnit () const;
    void                    setDefaultAngleUnit (Marble::AngleUnit angleUnit);
    QFont                   defaultFont () const;
    void                    setDefaultFont (const QFont& font);

signals:
    void                    projectionChanged (Marble::Projection);
    void                    mouseClickGeoPosition (qreal lon, qreal lat, Marble::GeoDataCoordinates::Unit);
    void                    framesPerSecond (qreal fps);

protected:
    virtual void            customPaint (Marble::GeoPainter* painter);

public:
    virtual ~MarbleMap ();
    void                    reload () const;
    Marble::ViewportParams*  viewport ();
    Marble::MapQuality      mapQuality () const;
    void                    setSize (const QSize& size);
    bool                    screenCoordinates (qreal lon, qreal lat, qreal& x, qreal& y) const;
    bool                    geoCoordinates (int x, int y, qreal& lon, qreal& lat, Marble::GeoDataCoordinates::Unit = Marble::GeoDataCoordinates::Degree) const;
    Marble::AbstractFloatItem*  floatItem (const QString& nameId) const;
    void                    centerOn (const qreal lon, const qreal lat);
signals:
    void                    themeChanged (const QString& theme);
    void                    mouseMoveGeoPosition (const QString&);
    void                    visibleLatLonAltBoxChanged (const Marble::GeoDataLatLonAltBox& visibleLatLonAltBox);
public:
    int                     tileZoomLevel () const;
    bool                    showBackground () const;
    QList<Marble::AbstractDataPlugin*>  dataPlugins () const;
    QList<Marble::AbstractDataPluginItem*>  whichItemAt (const QPoint& curpos) const;
    void                    addLayer (Marble::LayerInterface* layer);
    void                    removeLayer (Marble::LayerInterface* layer);
    void                    setShowBackground (bool visible);
    void                    downloadRegion (const QString& sourceDir, const QVector<Marble::TileCoordsPyramid>&);
signals:
    void                    tileLevelChanged (int level);
    void                    repaintNeeded (const QRegion& dirtyRegion = QRegion());
    void                    pluginSettingsChanged ();
    void                    renderPluginInitialized (Marble::RenderPlugin* renderPlugin);
public:
    void                    setMapQualityForViewContext (Marble::MapQuality qualityForViewContext, Marble::ViewContext viewContext);
    Marble::MapQuality      mapQuality (Marble::ViewContext viewContext) const;
    void                    setViewContext (Marble::ViewContext viewContext);
    Marble::ViewContext     viewContext () const;
    int                     preferredRadiusCeil (int radius);
    int                     preferredRadiusFloor (int radius);
//FIXME
//ig    QVector<const Marble::GeoDataPlacemark*>  whichFeatureAt (const QPoint&) const;
//ig    const Marble::ViewportParams*  viewport () const;
    bool                    showSunShading () const;
    bool                    showCityLights () const;
    bool                    showSunInZenith () const;
    void                    paint (Marble::GeoPainter& painter, const QRect& dirtyRect);
    void                    setShowSunShading (bool visible);
    void                    setShowCityLights (bool visible);
    void                    setShowSunInZenith (bool visible);
signals:
    void                    radiusChanged (int radius);
public:
//ig    const TextureLayer*     textureLayer () const;
};
// MarbleMap

};
// Marble


