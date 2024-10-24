// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Adam Dabrowski <adabrowski@piap.pl, adamdbrw@gmail.com>
//

#ifndef MARBLEQUICKITEM_H
#define MARBLEQUICKITEM_H

#include "Coordinate.h"
#include "GeoDataAccuracy.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "MapTheme.h"
#include "MarbleGlobal.h"
#include "MarbleMap.h"
#include "Placemark.h"
#include "PositionProviderPluginInterface.h"

#include <QPolygonF>
#include <QQuickPaintedItem>
#include <QSharedPointer>
#include <qqmlregistration.h>

namespace Marble
{
class GeoDataLatLonBox;
class GeoDataPlacemark;
class MarbleModel;
class MarbleInputHandler;
class MarbleQuickItemPrivate;

// Class is still being developed
class MarbleQuickItem : public QQuickPaintedItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MarbleItem)

    Q_ENUM(Projection)

    Q_PROPERTY(int mapWidth READ mapWidth WRITE setMapWidth NOTIFY mapWidthChanged)
    Q_PROPERTY(int mapHeight READ mapHeight WRITE setMapHeight NOTIFY mapHeightChanged)
    Q_PROPERTY(int zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(int minimumZoom READ minimumZoom NOTIFY minimumZoomChanged)
    Q_PROPERTY(int maximumZoom READ maximumZoom NOTIFY maximumZoomChanged)
    Q_PROPERTY(int radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal heading READ heading WRITE setHeading NOTIFY headingChanged)

    Q_PROPERTY(bool showFrameRate READ showFrameRate WRITE setShowFrameRate NOTIFY showFrameRateChanged)
    Q_PROPERTY(Projection projection READ projection WRITE setProjection NOTIFY projectionChanged)

    Q_PROPERTY(Marble::MapTheme *mapTheme READ mapTheme NOTIFY mapThemeChanged)
    Q_PROPERTY(QString mapThemeId READ mapThemeId WRITE setMapThemeId NOTIFY mapThemeIdChanged)
    Q_PROPERTY(bool showAtmosphere READ showAtmosphere WRITE setShowAtmosphere NOTIFY showAtmosphereChanged)
    Q_PROPERTY(bool showCompass READ showCompass WRITE setShowCompass NOTIFY showCompassChanged)
    Q_PROPERTY(bool showClouds READ showClouds WRITE setShowClouds NOTIFY showCloudsChanged)
    Q_PROPERTY(bool showCrosshairs READ showCrosshairs WRITE setShowCrosshairs NOTIFY showCrosshairsChanged)
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(bool showOverviewMap READ showOverviewMap WRITE setShowOverviewMap NOTIFY showOverviewMapChanged)
    Q_PROPERTY(bool showOtherPlaces READ showOtherPlaces WRITE setShowOtherPlaces NOTIFY showOtherPlacesChanged)
    Q_PROPERTY(bool showScaleBar READ showScaleBar WRITE setShowScaleBar NOTIFY showScaleBarChanged)
    Q_PROPERTY(bool showBackground READ showBackground WRITE setShowBackground NOTIFY showBackgroundChanged)
    Q_PROPERTY(bool showPositionMarker READ showPositionMarker WRITE setShowPositionMarker NOTIFY showPositionMarkerChanged)
    Q_PROPERTY(bool showPublicTransport READ showPublicTransport WRITE setShowPublicTransport NOTIFY showPublicTransportChanged)
    Q_PROPERTY(bool showOutdoorActivities READ showOutdoorActivities WRITE setShowOutdoorActivities NOTIFY showOutdoorActivitiesChanged)
    Q_PROPERTY(QString positionProvider READ positionProvider WRITE setPositionProvider NOTIFY positionProviderChanged)
    Q_PROPERTY(bool positionAvailable READ positionAvailable NOTIFY positionAvailableChanged)
    Q_PROPERTY(bool positionVisible READ positionVisible NOTIFY positionVisibleChanged)
    Q_PROPERTY(MarbleMap *marbleMap READ map NOTIFY marbleMapChanged)
    Q_PROPERTY(Placemark *currentPosition READ currentPosition NOTIFY currentPositionChanged)
    Q_PROPERTY(qreal speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(qreal angle READ angle NOTIFY angleChanged)
    Q_PROPERTY(bool inertialGlobeRotation READ inertialGlobeRotation WRITE setInertialGlobeRotation NOTIFY inertialGlobeRotationChanged)
    Q_PROPERTY(bool animationViewContext READ animationViewContext WRITE setAnimationViewContext NOTIFY animationViewContextChanged)
    Q_PROPERTY(bool animationsEnabled READ animationsEnabled WRITE setAnimationsEnabled NOTIFY animationsEnabledChanged)
    Q_PROPERTY(QQmlComponent *placemarkDelegate READ placemarkDelegate WRITE setPlacemarkDelegate NOTIFY placemarkDelegateChanged)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(bool invertColorEnabled READ invertColorEnabled WRITE setInvertColorEnabled NOTIFY invertColorEnabledChanged)
    Q_PROPERTY(bool workOffline READ workOffline WRITE setWorkOffline NOTIFY workOfflineChanged)

public:
    explicit MarbleQuickItem(QQuickItem *parent = nullptr);

    enum Projection {
        Spherical = Marble::Spherical,
        Equirectangular = Marble::Equirectangular,
        Mercator = Marble::Mercator,
        Gnomonic = Marble::Gnomonic,
        Stereographic = Marble::Stereographic,
        LambertAzimuthal = Marble::LambertAzimuthal,
        AzimuthalEquidistant = Marble::AzimuthalEquidistant,
        VerticalPerspective = Marble::VerticalPerspective
    };

    MarbleInputHandler *inputHandler();
    int zoom() const;
    int radius() const;

public Q_SLOTS:
    void goHome();
    void setZoom(int zoom, FlyToMode mode = Instant);
    Q_INVOKABLE void setZoomToMaximumLevel();
    void setRadius(int radius);
    void centerOn(const GeoDataPlacemark &placemark, bool animated = false);
    void centerOn(const GeoDataLatLonBox &box, bool animated = false);
    void centerOn(const GeoDataCoordinates &coordinate);
    void centerOn(qreal longitude, qreal latitude);
    Q_INVOKABLE void centerOnCoordinates(qreal longitude, qreal latitude);
    Q_INVOKABLE void centerOnCurrentPosition();
    Q_INVOKABLE void selectPlacemarkAt(int x, int y);

    void zoomIn(FlyToMode mode = Automatic);
    void zoomOut(FlyToMode mode = Automatic);

    Q_INVOKABLE void handlePinchStarted(const QPointF &point);
    Q_INVOKABLE void handlePinchFinished(const QPointF &point);
    Q_INVOKABLE void handlePinchUpdated(const QPointF &point, qreal scale);

    void setMapWidth(int mapWidth);
    void setMapHeight(int mapHeight);
    void setShowFrameRate(bool showFrameRate);
    void setProjection(Projection projection);
    void setMapThemeId(const QString &mapThemeId);
    void setShowAtmosphere(bool showAtmosphere);
    void setShowCompass(bool showCompass);
    void setShowClouds(bool showClouds);
    void setShowCrosshairs(bool showCrosshairs);
    void setShowGrid(bool showGrid);
    void setShowOverviewMap(bool showOverviewMap);
    void setShowOtherPlaces(bool showOtherPlaces);
    void setShowScaleBar(bool showScaleBar);
    void setShowBackground(bool showBackground);
    void setShowPositionMarker(bool showPositionMarker);
    void setShowPublicTransport(bool showPublicTransport);
    void setShowOutdoorActivities(bool showOutdoorActivities);
    void setPositionProvider(const QString &positionProvider);

    void setInertialGlobeRotation(bool inertialGlobeRotation);
    void setAnimationViewContext(bool animationViewContext);
    void setAnimationsEnabled(bool animationsEnabled);

    void setPluginSetting(const QString &plugin, const QString &key, const QString &value);

    void setPropertyEnabled(const QString &property, bool enabled);
    bool isPropertyEnabled(const QString &property) const;

    void setWorkOffline(bool enabled);

    Q_INVOKABLE void setInvertColorEnabled(bool enabled, const QString &blending = QStringLiteral("InvertColorBlending"));

    Q_INVOKABLE void setShowRuntimeTrace(bool showRuntimeTrace);
    Q_INVOKABLE void setShowDebugPolygons(bool showDebugPolygons);
    Q_INVOKABLE void setShowDebugPlacemarks(bool showDebugPlacemarks);
    Q_INVOKABLE void setShowDebugBatches(bool showDebugBatches);

    void setPlacemarkDelegate(QQmlComponent *placemarkDelegate);

    Q_INVOKABLE void loadSettings();
    Q_INVOKABLE void writeSettings();

    Q_INVOKABLE void reloadTiles();
    Q_INVOKABLE void highlightRouteRelation(qint64 osmId, bool enabled);
    Q_INVOKABLE void setRelationTypeVisible(const QString &relationType, bool visible);
    Q_INVOKABLE bool isRelationTypeVisible(const QString &relationType) const;

    void setHeading(qreal heading);
    void setHoverEnabled(bool hoverEnabled);
    qreal centerLongitude() const;
    qreal centerLatitude() const;

public:
    void paint(QPainter *painter) override;

    // QQmlParserStatus interface
public:
    void classBegin() override;
    void componentComplete() override;

    void hoverMoveEvent(QHoverEvent *event) override;

public:
    virtual bool layersEventFilter(QObject *o, QEvent *e);

    int mapWidth() const;
    int mapHeight() const;
    qreal heading() const;

    bool showFrameRate() const;
    Projection projection() const;
    QString mapThemeId() const;
    Marble::MapTheme *mapTheme() const;
    bool showAtmosphere() const;
    bool showCompass() const;
    bool showClouds() const;
    bool showCrosshairs() const;
    bool showGrid() const;
    bool showOverviewMap() const;
    bool showOtherPlaces() const;
    bool showScaleBar() const;
    bool showBackground() const;
    bool showPositionMarker() const;
    bool showPublicTransport() const;
    bool showOutdoorActivities() const;
    QString positionProvider() const;
    bool positionAvailable() const;
    bool positionVisible() const;
    Q_INVOKABLE qreal distanceFromPointToCurrentLocation(const QPoint &position) const;
    Q_INVOKABLE qreal angleFromPointToCurrentLocation(const QPoint &position) const;
    Placemark *currentPosition() const;
    Q_INVOKABLE QPointF screenCoordinatesFromCoordinate(Coordinate *coordinate) const;
    Q_INVOKABLE QPointF screenCoordinatesFromGeoDataCoordinates(const GeoDataCoordinates &coordinates) const;
    bool screenCoordinatesFromGeoDataLineString(const GeoDataLineString &lineString, QList<QPolygonF *> &polygons) const;

    bool screenCoordinatesToGeoDataCoordinates(const QPoint &point, GeoDataCoordinates &coordinates);
    Q_INVOKABLE bool screenCoordinatesToCoordinate(const QPoint &point, Coordinate *coordinate);
    qreal speed() const;
    qreal angle() const;

    MarbleModel *model();
    const MarbleModel *model() const;

    MarbleMap *map();
    const MarbleMap *map() const;

    bool inertialGlobeRotation() const;
    bool animationViewContext() const;
    bool animationsEnabled() const;

    QQmlComponent *placemarkDelegate() const;
    void reverseGeocoding(const QPoint &point);

    bool hoverEnabled() const;
    Q_INVOKABLE bool invertColorEnabled();
    bool workOffline();

    Q_INVOKABLE void moveUp();
    Q_INVOKABLE void moveDown();
    Q_INVOKABLE void moveLeft();
    Q_INVOKABLE void moveRight();

    int minimumZoom() const;

    int maximumZoom() const;

Q_SIGNALS:
    void mapWidthChanged(int mapWidth);
    void mapHeightChanged(int mapHeight);
    void showFrameRateChanged(bool showFrameRate);
    void projectionChanged(Projection projection);
    void mapThemeIdChanged(const QString &mapThemeId);
    void mapThemeChanged();
    void showAtmosphereChanged(bool showAtmosphere);
    void showCompassChanged(bool showCompass);
    void showCloudsChanged(bool showClouds);
    void showCrosshairsChanged(bool showCrosshairs);
    void showGridChanged(bool showGrid);
    void showOverviewMapChanged(bool showOverviewMap);
    void showOtherPlacesChanged(bool showOtherPlaces);
    void showScaleBarChanged(bool showScaleBar);
    void showBackgroundChanged(bool showBackground);
    void showPositionMarkerChanged(bool showPositionMarker);
    void showPublicTransportChanged(bool showPublicTransport);
    void showOutdoorActivitiesChanged(bool showOutdoorActivities);
    void positionProviderChanged(const QString &positionProvider);
    void positionAvailableChanged(bool positionAvailable);
    void positionVisibleChanged(bool positionVisible);
    void marbleMapChanged();
    void visibleLatLonAltBoxChanged();
    void currentPositionChanged(Placemark *currentPosition);
    void angleChanged();
    void speedChanged();
    void headingChanged(qreal heading);
    void zoomChanged();
    void radiusChanged(int radius);
    void inertialGlobeRotationChanged(bool inertialGlobeRotation);
    void animationViewContextChanged(bool animationViewContext);
    void placemarkDelegateChanged(QQmlComponent *placemarkDelegate);

    void animationsEnabledChanged(bool animationsEnabled);

    void hoverEnabledChanged(bool hoverEnabled);

    void lmbMenuRequested(const QPoint &point);
    void rmbMenuRequested(const QPoint &point);
    void hoverPositionChanged(const QPoint &point);

    void minimumZoomChanged(int minimumZoom);
    void maximumZoomChanged(int maximumZoom);

    void invertColorEnabledChanged(bool enabled);
    void workOfflineChanged();

    void geoItemUpdateRequested();

protected:
    QObject *getEventFilter() const;
    void pinch(const QPointF &center, qreal scale, Qt::GestureState state);

private Q_SLOTS:
    void resizeMap();
    void positionDataStatusChanged(PositionProviderStatus status);
    void positionChanged(const GeoDataCoordinates &, GeoDataAccuracy);
    void updatePositionVisibility();
    void updateCurrentPosition(const GeoDataCoordinates &coordinates);
    void updatePlacemarks();
    void handleReverseGeocoding(const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark);
    void handleVisibleLatLonAltBoxChanged(const GeoDataLatLonAltBox &latLonAltBox);

private:
    using MarbleQuickItemPrivatePtr = QSharedPointer<MarbleQuickItemPrivate>;
    MarbleQuickItemPrivatePtr d;
    friend class MarbleQuickItemPrivate;
};
}

#endif // MARBLEQUICKITEM_H
