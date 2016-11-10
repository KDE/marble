//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Adam Dabrowski <adabrowski@piap.pl> <adamdbrw@gmail.com>
//

#ifndef MARBLEQUICKITEM_H
#define MARBLEQUICKITEM_H

#include "marble_declarative_export.h"
#include <QSharedPointer>
#include <QQuickPaintedItem>
#include "GeoDataAccuracy.h"
#include "MarbleGlobal.h"
#include "PositionProviderPluginInterface.h"
#include "MarbleMap.h"
#include "Placemark.h"
#include "Coordinate.h"

namespace Marble
{
    class GeoDataLatLonBox;
    class GeoDataPlacemark;
    class MarbleModel;
    class MarbleInputHandler;
    class MarbleQuickItemPrivate;

    //Class is still being developed
    class MARBLE_DECLARATIVE_EXPORT MarbleQuickItem : public QQuickPaintedItem
    {
    Q_OBJECT

        Q_ENUMS(Projection)

        Q_PROPERTY(int mapWidth READ mapWidth WRITE setMapWidth NOTIFY mapWidthChanged)
        Q_PROPERTY(int mapHeight READ mapHeight WRITE setMapHeight NOTIFY mapHeightChanged)
        Q_PROPERTY(int zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
        Q_PROPERTY(int radius READ radius WRITE setRadius NOTIFY radiusChanged)
        Q_PROPERTY(bool showFrameRate READ showFrameRate WRITE setShowFrameRate NOTIFY showFrameRateChanged)
        Q_PROPERTY(Projection projection READ projection WRITE setProjection NOTIFY projectionChanged)
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
        Q_PROPERTY(QString positionProvider READ positionProvider WRITE setPositionProvider NOTIFY positionProviderChanged)
        Q_PROPERTY(bool positionAvailable READ positionAvailable NOTIFY positionAvailableChanged)
        Q_PROPERTY(bool positionVisible READ positionVisible NOTIFY positionVisibleChanged)
        Q_PROPERTY(MarbleMap* marbleMap READ map NOTIFY marbleMapChanged)
        Q_PROPERTY(Placemark*  currentPosition READ currentPosition NOTIFY currentPositionChanged)
        Q_PROPERTY(qreal speed READ speed NOTIFY speedChanged)
        Q_PROPERTY(qreal angle READ angle NOTIFY angleChanged)
        Q_PROPERTY(bool inertialGlobeRotation READ inertialGlobeRotation WRITE setInertialGlobeRotation NOTIFY inertialGlobeRotationChanged)
        Q_PROPERTY(QQmlComponent* placemarkDelegate READ placemarkDelegate WRITE setPlacemarkDelegate NOTIFY placemarkDelegateChanged)

    public:
        explicit MarbleQuickItem(QQuickItem *parent = 0);

        enum Projection{
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
        void centerOn(const GeoDataPlacemark& placemark, bool animated = false);
        void centerOn(const GeoDataLatLonBox& box, bool animated = false);
        void centerOn(const GeoDataCoordinates& coordinate);
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
        void setMapThemeId(const QString& mapThemeId);
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
        void setPositionProvider(const QString & positionProvider);

        void setInertialGlobeRotation(bool inertialGlobeRotation);

        void setPluginSetting(const QString &plugin, const QString &key, const QString &value);

        void setPropertyEnabled(const QString &property, bool enabled);
        bool isPropertyEnabled(const QString &property) const;

        Q_INVOKABLE void setShowRuntimeTrace(bool showRuntimeTrace);
        Q_INVOKABLE void setShowDebugPolygons(bool showDebugPolygons);
        Q_INVOKABLE void setShowDebugPlacemarks(bool showDebugPlacemarks);
        Q_INVOKABLE void setShowDebugBatches(bool showDebugBatches);

        void setPlacemarkDelegate(QQmlComponent* placemarkDelegate);

        Q_INVOKABLE void loadSettings();
        Q_INVOKABLE void writeSettings();

    public:
        void paint(QPainter *painter);

    // QQmlParserStatus interface
    public:
        void classBegin();
        void componentComplete();

    public:
        virtual bool layersEventFilter(QObject *o, QEvent *e);

        int mapWidth() const;
        int mapHeight() const;
        bool showFrameRate() const;
        Projection projection() const;
        QString mapThemeId() const;
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
        QString positionProvider() const;
        bool positionAvailable() const;
        bool positionVisible();
        Q_INVOKABLE qreal distanceFromPointToCurrentLocation(const QPoint & position) const;
        Q_INVOKABLE qreal angleFromPointToCurrentLocation(const QPoint & position) const;
        Placemark* currentPosition() const;
        Q_INVOKABLE QPointF screenCoordinatesFromCoordinate(Coordinate * coordinate) const;
        qreal speed() const;
        qreal angle() const;

        MarbleModel* model();
        const MarbleModel* model() const;

        MarbleMap* map();
        const MarbleMap* map() const;

        bool inertialGlobeRotation() const;
        QQmlComponent* placemarkDelegate() const;
        void reverseGeocoding(const QPoint &point);

    Q_SIGNALS:
        void mapWidthChanged(int mapWidth);
        void mapHeightChanged(int mapHeight);
        void showFrameRateChanged(bool showFrameRate);
        void projectionChanged(Projection projection);
        void mapThemeIdChanged(const QString& mapThemeId);
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
        void positionProviderChanged(const QString & positionProvider);
        void positionAvailableChanged(bool positionAvailable);
        void positionVisibleChanged(bool positionVisible);
        void marbleMapChanged();
        void visibleLatLonAltBoxChanged();
        void currentPositionChanged(Placemark* currentPosition);
        void angleChanged();
        void speedChanged();
        void zoomChanged();
        void radiusChanged(int radius);
        void inertialGlobeRotationChanged(bool inertialGlobeRotation);
        void placemarkDelegateChanged(QQmlComponent* placemarkDelegate);

    protected:
        QObject *getEventFilter() const;
        void pinch(const QPointF& center, qreal scale, Qt::GestureState state);

    private Q_SLOTS:
        void resizeMap();
        void positionDataStatusChanged(PositionProviderStatus status);
        void positionChanged(const GeoDataCoordinates &, GeoDataAccuracy);
        void updatePositionVisibility();
        void updateCurrentPosition(const GeoDataCoordinates & coordinates);
        void updatePlacemarks();
        void handleReverseGeocoding(const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark);

    private:
        typedef QSharedPointer<MarbleQuickItemPrivate> MarbleQuickItemPrivatePtr;
        MarbleQuickItemPrivatePtr d;
        friend class MarbleQuickItemPrivate;
    };
}

#endif // MARBLEQUICKITEM_H
