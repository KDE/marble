//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Adam Dabrowski <adabrowski@piap.pl> <adamdbrw@gmail.com>
//


#include <MarbleQuickItem.h>
#include <QPainter>
#include <QPaintDevice>
#include <QtMath>
#include <QQmlContext>
#include <QSettings>

#include <MarbleModel.h>
#include <MarbleMap.h>
#include <ViewportParams.h>
#include <GeoPainter.h>
#include <GeoDataLookAt.h>
#include <Planet.h>
#include <MarbleAbstractPresenter.h>
#include <AbstractFloatItem.h>
#include <MarbleInputHandler.h>
#include <PositionTracking.h>
#include <PositionProviderPlugin.h>
#include <PluginManager.h>
#include <RenderPlugin.h>
#include <MarbleMath.h>
#include <GeoDataLatLonAltBox.h>
#include <GeoDataCoordinates.h>
#include <GeoDataTypes.h>
#include <ReverseGeocodingRunnerManager.h>
#include <routing/RoutingManager.h>
#include <routing/RoutingModel.h>
#include <routing/Route.h>
#include <BookmarkManager.h>

namespace Marble
{
    //TODO - move to separate files
    class QuickItemSelectionRubber : public AbstractSelectionRubber
    { //TODO: support rubber selection in MarbleQuickItem
    public:
        void show() { m_visible = true; }
        void hide() { m_visible = false; }
        bool isVisible() const { return m_visible; }
        const QRect &geometry() const { return m_geometry; }
        void setGeometry(const QRect &/*geometry*/) {}
    private:
        QRect m_geometry;
        bool m_visible;
    };

    //TODO - implement missing functionalities
    class MarbleQuickInputHandler : public MarbleDefaultInputHandler
    {
    public:
        MarbleQuickInputHandler(MarbleAbstractPresenter *marblePresenter, MarbleQuickItem *marbleQuick)
            : MarbleDefaultInputHandler(marblePresenter)
            ,m_marbleQuick(marbleQuick)
        {
            setInertialEarthRotationEnabled(false); //Disabled by default, it's buggy. TODO - fix
        }

        bool acceptMouse()
        {
            return true;
        }

        void pinch(QPointF center, qreal scale, Qt::GestureState state)
        {   //TODO - this whole thing should be moved to MarbleAbstractPresenter
            (void)handlePinch(center, scale, state);
        }

        void handleMouseButtonPressAndHold(const QPoint &position)
        {
            m_marbleQuick->reverseGeocoding(position);
        }

    private Q_SLOTS:
        void showLmbMenu(int x, int y)
        {
            m_marbleQuick->selectPlacemarkAt(x, y);
        }

        void showRmbMenu(int, int) {}
        void openItemToolTip() {}
        void setCursor(const QCursor &cursor)
        {
            m_marbleQuick->setCursor(cursor);
        }

    private Q_SLOTS:
        void installPluginEventFilter(RenderPlugin *) {}

    private:
        bool layersEventFilter(QObject *o, QEvent *e)
        {
            return m_marbleQuick->layersEventFilter(o, e);
        }

        //empty - don't check. It would be invalid with quick items
        void checkReleasedMove(QMouseEvent *) {}

        bool handleTouch(QTouchEvent *event)
        {
            if (event->touchPoints().count() > 1)
            {   //not handling multi-touch at all, let PinchArea or MultiPointTouchArea take care of it
                return false;
            }

            if (event->touchPoints().count() == 1)
            {   //handle - but do not accept. I.e. pinchArea still needs to get this
                QTouchEvent::TouchPoint p = event->touchPoints().at(0);
                if (event->type() == QEvent::TouchBegin)
                {
                    QMouseEvent press(QMouseEvent::MouseButtonPress, p.pos(),
                                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                    handleMouseEvent(&press);
                }
                else if (event->type() == QEvent::TouchUpdate)
                {
                    QMouseEvent move(QMouseEvent::MouseMove, p.pos(),
                                      Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
                    handleMouseEvent(&move);
                }
                else if (event->type() == QEvent::TouchEnd)
                {
                    QMouseEvent release(QMouseEvent::MouseButtonRelease, p.pos(),
                                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                    handleMouseEvent(&release);
                }
            }
            return false;
        }

        AbstractSelectionRubber *selectionRubber()
        {
            return &m_selectionRubber;
        }

        MarbleQuickItem *m_marbleQuick;
        QuickItemSelectionRubber m_selectionRubber;
        bool m_usePinchArea;
    };

    class MarbleQuickItemPrivate
    {
    public:
        explicit MarbleQuickItemPrivate(MarbleQuickItem *marble) :
            m_marble(marble),
            m_model(),
            m_map(&m_model),
            m_presenter(&m_map),
            m_positionVisible(false),
            m_currentPosition(marble),
            m_inputHandler(&m_presenter, marble),
            m_placemarkDelegate(nullptr),
            m_placemarkItem(nullptr),
            m_placemark(nullptr),
            m_reverseGeocoding(&m_model)
        {
            m_currentPosition.setName(QObject::tr("Current Location"));
        }

    private:
        MarbleQuickItem *m_marble;
        friend class MarbleQuickItem;
        MarbleModel m_model;
        MarbleMap m_map;
        MarbleAbstractPresenter m_presenter;
        bool m_positionVisible;
        Placemark m_currentPosition;

        MarbleQuickInputHandler m_inputHandler;
        QQmlComponent* m_placemarkDelegate;
        QQuickItem* m_placemarkItem;
        Placemark* m_placemark;
        ReverseGeocodingRunnerManager m_reverseGeocoding;
    };

    MarbleQuickItem::MarbleQuickItem(QQuickItem *parent) : QQuickPaintedItem(parent)
      ,d(new MarbleQuickItemPrivate(this))
    {
        setRenderTarget(QQuickPaintedItem::FramebufferObject);
        setOpaquePainting(true);
        qRegisterMetaType<Placemark*>("Placemark*");

        foreach (AbstractFloatItem *item, d->m_map.floatItems()) {
            if (item->nameId() == QLatin1String("license")) {
                item->setPosition(QPointF(5.0, -10.0));
            } else {
                item->hide();
            }
        }

        d->m_model.positionTracking()->setTrackVisible(false);

        connect(&d->m_map, SIGNAL(repaintNeeded(QRegion)), this, SLOT(update()));
        connect(this, SIGNAL(widthChanged()), this, SLOT(resizeMap()));
        connect(this, SIGNAL(heightChanged()), this, SLOT(resizeMap()));
        connect(&d->m_map, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)), this, SLOT(updatePositionVisibility()));
        connect(&d->m_map, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)), this, SIGNAL(visibleLatLonAltBoxChanged()));
        connect(&d->m_map, SIGNAL(radiusChanged(int)), this, SIGNAL(radiusChanged(int)));
        connect(&d->m_map, SIGNAL(radiusChanged(int)), this, SIGNAL(zoomChanged()));
        connect(&d->m_reverseGeocoding, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)),
                this, SLOT(handleReverseGeocoding(GeoDataCoordinates,GeoDataPlacemark)));

        setAcceptedMouseButtons(Qt::AllButtons);
        installEventFilter(&d->m_inputHandler);
    }

    void MarbleQuickItem::resizeMap()
    {
        const int minWidth = 100;
        const int minHeight = 100;
        int newWidth = width() > minWidth ? (int)width() : minWidth;
        int newHeight = height() > minHeight ? (int)height() : minHeight;

        d->m_map.setSize(newWidth, newHeight);
        update();
        updatePositionVisibility();
    }

    void MarbleQuickItem::positionDataStatusChanged(PositionProviderStatus status)
    {
        if (status == PositionProviderStatusAvailable) {
            emit positionAvailableChanged(true);
        }
        else {
            emit positionAvailableChanged(false);
        }
        updatePositionVisibility();
    }

    void MarbleQuickItem::positionChanged(const GeoDataCoordinates &, GeoDataAccuracy)
    {
       updatePositionVisibility();
    }

    void MarbleQuickItem::updatePositionVisibility()
    {
        updatePlacemarks();
        bool isVisible = false;
        if ( positionAvailable() ) {
            if ( d->m_map.viewport()->viewLatLonAltBox().contains(d->m_model.positionTracking()->currentLocation()) ) {
                isVisible = true;
            }
        }

        if ( isVisible != d->m_positionVisible ) {
            d->m_positionVisible = isVisible;
            emit positionVisibleChanged( isVisible );
        }
    }

    void MarbleQuickItem::updateCurrentPosition(const GeoDataCoordinates &coordinates)
    {
        d->m_currentPosition.placemark().setCoordinate(coordinates);
        emit currentPositionChanged(&d->m_currentPosition);
    }

    void MarbleQuickItem::updatePlacemarks()
    {
        if (!d->m_placemarkDelegate || !d->m_placemark) {
            return;
        }

        if (!d->m_placemarkItem) {
            QQmlContext * context = new QQmlContext(qmlContext(d->m_placemarkDelegate));
            QObject * component = d->m_placemarkDelegate->create(context);
            d->m_placemarkItem = qobject_cast<QQuickItem*>( component );
            if (d->m_placemarkItem) {
                d->m_placemarkItem->setParentItem( this );
                d->m_placemarkItem->setProperty("placemark", QVariant::fromValue(d->m_placemark));
            } else {
                delete component;
                return;
            }
        }

        qreal x = 0;
        qreal y = 0;
        const bool visible = d->m_map.viewport()->screenCoordinates(d->m_placemark->placemark().coordinate(), x, y);
        d->m_placemarkItem->setVisible(visible);
        if (visible) {
            d->m_placemarkItem->setProperty("xPos", QVariant(x));
            d->m_placemarkItem->setProperty("yPos", QVariant(y));
        }
    }

    void MarbleQuickItem::handleReverseGeocoding(const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark)
    {
        if (d->m_placemark && d->m_placemark->placemark().coordinate() == coordinates) {
            d->m_placemark->setGeoDataPlacemark(placemark);
            updatePlacemarks();
        }
    }

    void MarbleQuickItem::paint(QPainter *painter)
    {   //TODO - much to be done here still, i.e paint !enabled version
        QPaintDevice *paintDevice = painter->device();
        QRect rect = contentsBoundingRect().toRect();

        painter->end();
        {
            GeoPainter geoPainter(paintDevice, d->m_map.viewport(), d->m_map.mapQuality());
            d->m_map.paint(geoPainter, rect);
        }
        painter->begin(paintDevice);
    }

    void MarbleQuickItem::classBegin()
    {
    }

    void MarbleQuickItem::componentComplete()
    {
    }

    int MarbleQuickItem::mapWidth() const
    {
       return d->m_map.width();
    }

    int MarbleQuickItem::mapHeight() const
    {
        return d->m_map.height();
    }

    bool MarbleQuickItem::showFrameRate() const
    {
        return d->m_map.showFrameRate();
    }

    MarbleQuickItem::Projection MarbleQuickItem::projection() const
    {
        return (Projection)d->m_map.projection();
    }

    QString MarbleQuickItem::mapThemeId() const
    {
        return d->m_map.mapThemeId();
    }

    bool MarbleQuickItem::showAtmosphere() const
    {
        return d->m_map.showAtmosphere();
    }

    bool MarbleQuickItem::showCompass() const
    {
        return d->m_map.showCompass();
    }

    bool MarbleQuickItem::showClouds() const
    {
        return d->m_map.showClouds();
    }

    bool MarbleQuickItem::showCrosshairs() const
    {
        return d->m_map.showCrosshairs();
    }

    bool MarbleQuickItem::showGrid() const
    {
        return d->m_map.showGrid();
    }

    bool MarbleQuickItem::showOverviewMap() const
    {
        return d->m_map.showOverviewMap();
    }

    bool MarbleQuickItem::showOtherPlaces() const
    {
        return d->m_map.showOtherPlaces();
    }

    bool MarbleQuickItem::showScaleBar() const
    {
        return d->m_map.showScaleBar();
    }

    bool MarbleQuickItem::showBackground() const
    {
        return d->m_map.showBackground();
    }

    bool MarbleQuickItem::showPositionMarker() const
    {
        QList<RenderPlugin *> plugins = d->m_map.renderPlugins();
        foreach (const RenderPlugin * plugin, plugins) {
            if (plugin->nameId() == QLatin1String("positionMarker")) {
                return plugin->visible();
            }
        }
        return false;
    }

    QString MarbleQuickItem::positionProvider() const
    {
        if ( d->m_model.positionTracking()->positionProviderPlugin() ) {
            return d->m_model.positionTracking()->positionProviderPlugin()->nameId();
        }

        return QString();
    }

    MarbleModel* MarbleQuickItem::model()
    {
        return &d->m_model;
    }

    const MarbleModel* MarbleQuickItem::model() const
    {
        return &d->m_model;
    }

    MarbleMap* MarbleQuickItem::map()
    {
        return &d->m_map;
    }

    const MarbleMap* MarbleQuickItem::map() const
    {
        return &d->m_map;
    }

    bool MarbleQuickItem::inertialGlobeRotation() const
    {
        return d->m_inputHandler.inertialEarthRotationEnabled();
    }

    QQmlComponent *MarbleQuickItem::placemarkDelegate() const
    {
        return d->m_placemarkDelegate;
    }

    void MarbleQuickItem::reverseGeocoding(const QPoint &point)
    {
        qreal lon, lat;
        d->m_map.viewport()->geoCoordinates(point.x(), point.y(), lon, lat);
        auto const coordinates = GeoDataCoordinates(lon, lat, 0.0, GeoDataCoordinates::Degree);
        delete d->m_placemarkItem;
        d->m_placemarkItem = nullptr;
        delete d->m_placemark;
        d->m_placemark = new Placemark(this);
        d->m_placemark->placemark().setCoordinate(coordinates);
        d->m_reverseGeocoding.reverseGeocoding(coordinates);
    }

    qreal MarbleQuickItem::speed() const
    {
        return d->m_model.positionTracking()->speed();
    }

    qreal MarbleQuickItem::angle() const
    {
        bool routeExists = d->m_model.routingManager()->routingModel()->route().distance() != 0;
        bool onRoute = !d->m_model.routingManager()->routingModel()->deviatedFromRoute();
        if ( routeExists && onRoute) {
            GeoDataCoordinates curPoint = d->m_model.positionTracking()->positionProviderPlugin()->position();
            return d->m_model.routingManager()->routingModel()->route().currentSegment().projectedDirection(curPoint);
        } else {
            return d->m_model.positionTracking()->direction();
        }
    }

    bool MarbleQuickItem::positionAvailable() const
    {
        return d->m_model.positionTracking()->status() == PositionProviderStatusAvailable;
    }

    bool MarbleQuickItem::positionVisible()
    {
        return d->m_positionVisible;
    }

    qreal MarbleQuickItem::distanceFromPointToCurrentLocation(const QPoint & position) const
    {
        if ( positionAvailable() ) {
            qreal lon1;
            qreal lat1;
            d->m_map.viewport()->geoCoordinates(position.x(), position.y(), lon1, lat1, GeoDataCoordinates::Radian );

            GeoDataCoordinates currentCoordinates = d->m_model.positionTracking()->currentLocation();
            qreal lon2 = currentCoordinates.longitude();
            qreal lat2 = currentCoordinates.latitude();

            return distanceSphere(lon1, lat1, lon2, lat2) * d->m_model.planetRadius();
        }
        return 0;
    }

    qreal MarbleQuickItem::angleFromPointToCurrentLocation( const QPoint & position ) const
    {
        if ( positionAvailable() ) {
            qreal x, y;
            PositionTracking const * positionTracking = d->m_model.positionTracking();
            map()->viewport()->screenCoordinates( positionTracking->currentLocation(), x, y );
            return atan2( y-position.y(), x-position.x() ) * RAD2DEG;
        }
        return 0;
    }

    Placemark * MarbleQuickItem::currentPosition() const
    {
        return &d->m_currentPosition;
    }

    QPointF MarbleQuickItem::screenCoordinatesFromCoordinate(Coordinate * coordinate) const
    {
        qreal x;
        qreal y;
        d->m_map.viewport()->screenCoordinates(coordinate->coordinates(), x, y);
        return QPointF(x, y);
    }

    void MarbleQuickItem::setRadius(int radius)
    {
        d->m_map.setRadius(radius);
    }

    void MarbleQuickItem::setZoom(int newZoom, FlyToMode mode)
    {
        d->m_presenter.setZoom(newZoom, mode);
    }

    void MarbleQuickItem::setZoomToMaximumLevel()
    {
        d->m_presenter.setZoom(d->m_map.maximumZoom());
    }

    void MarbleQuickItem::centerOn(const GeoDataPlacemark& placemark, bool animated)
    {
        d->m_presenter.centerOn(placemark, animated);
    }

    void MarbleQuickItem::centerOn(const GeoDataLatLonBox& box, bool animated)
    {
        d->m_presenter.centerOn(box, animated);
    }

    void MarbleQuickItem::centerOn(const GeoDataCoordinates &coordinate)
    {
        GeoDataLookAt target = d->m_presenter.lookAt();
        target.setCoordinates(coordinate);
        d->m_presenter.flyTo(target, Automatic);
    }

    void MarbleQuickItem::centerOn(qreal longitude, qreal latitude)
    {
        d->m_presenter.centerOn(longitude, latitude);
    }

    void MarbleQuickItem::centerOnCoordinates(qreal longitude, qreal latitude) {
        centerOn(longitude, latitude);
    }

    void MarbleQuickItem::centerOnCurrentPosition()
    {
        GeoDataCoordinates coordinates = d->m_model.positionTracking()->currentLocation();
        if ( coordinates == GeoDataCoordinates() ) {
            return;
        }

        d->m_presenter.centerOn(coordinates, true);
        if (d->m_presenter.zoom() < 3000) {
            d->m_presenter.setZoom(3500);
        }
    }

    void MarbleQuickItem::selectPlacemarkAt(int x, int y)
    {
        auto features = d->m_map.whichFeatureAt(QPoint(x, y));
        if (features.empty()) {
            features = d->m_map.whichBuildingAt(QPoint(x, y));
        }
        QVector<GeoDataPlacemark const *> placemarks;
        foreach(auto feature, features) {
            if (feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
                placemarks << static_cast<const GeoDataPlacemark*>(feature);
            }
        }

        // Select bookmarks only if nothing else is found
        qSort(placemarks.begin(), placemarks.end(), [] (GeoDataPlacemark const *a, GeoDataPlacemark const *b) {
            int const left = a->visualCategory() == GeoDataPlacemark::Bookmark ? -1 : a->visualCategory();
            int const right = b->visualCategory() == GeoDataPlacemark::Bookmark ? -1 : b->visualCategory();
            return left > right;
        });

        foreach(auto placemark, placemarks) {
            if (d->m_placemark && placemark->coordinate() == d->m_placemark->placemark().coordinate()) {
                d->m_placemark->deleteLater();
                d->m_placemark = nullptr;
            } else {
                d->m_placemark->deleteLater();
                d->m_placemark = new Placemark(this);
                d->m_placemark->setGeoDataPlacemark(*placemark);
            }
            delete d->m_placemarkItem;
            d->m_placemarkItem = nullptr;
            updatePlacemarks();
            return;
        }

        if (d->m_placemark) {
            d->m_placemark->deleteLater();
            d->m_placemark = nullptr;
            delete d->m_placemarkItem;
            d->m_placemarkItem = nullptr;
            updatePlacemarks();
        }
    }

    void MarbleQuickItem::goHome()
    {
        d->m_presenter.goHome();
    }

    void MarbleQuickItem::zoomIn(FlyToMode mode)
    {
        d->m_presenter.zoomIn(mode);
    }

    void MarbleQuickItem::zoomOut(FlyToMode mode)
    {
        d->m_presenter.zoomOut(mode);
    }

    void MarbleQuickItem::handlePinchStarted(const QPointF &point)
    {
        pinch(point, 1, Qt::GestureStarted);
    }

    void MarbleQuickItem::handlePinchFinished(const QPointF &point)
    {
        pinch(point, 1, Qt::GestureFinished);
    }

    void MarbleQuickItem::handlePinchUpdated(const QPointF &point, qreal scale)
    {
        scale = sqrt(sqrt(scale));
        scale = qBound(static_cast<qreal>(0.5), scale, static_cast<qreal>(2.0));
        pinch(point, scale, Qt::GestureUpdated);
    }

    void MarbleQuickItem::setMapWidth(int mapWidth)
    {
        if (d->m_map.width() == mapWidth) {
            return;
        }

        d->m_map.setSize(mapWidth, mapHeight());
        emit mapWidthChanged(mapWidth);
    }

    void MarbleQuickItem::setMapHeight(int mapHeight)
    {
        if (this->mapHeight() == mapHeight) {
            return;
        }

        d->m_map.setSize(mapWidth(), mapHeight);
        emit mapHeightChanged(mapHeight);
    }

    void MarbleQuickItem::setShowFrameRate(bool showFrameRate)
    {
        if (this->showFrameRate() == showFrameRate) {
            return;
        }

        d->m_map.setShowFrameRate(showFrameRate);
        emit showFrameRateChanged(showFrameRate);
    }

    void MarbleQuickItem::setProjection(Projection projection)
    {
        if (this->projection() == projection) {
            return;
        }

        d->m_map.setProjection((Marble::Projection)projection);
        emit projectionChanged(projection);
    }

    void MarbleQuickItem::setMapThemeId(const QString& mapThemeId)
    {
        if (this->mapThemeId() == mapThemeId) {
            return;
        }

        bool const showCompass = d->m_map.showCompass();
        bool const showOverviewMap = d->m_map.showOverviewMap();
        bool const showOtherPlaces = d->m_map.showOtherPlaces();
        bool const showGrid = d->m_map.showGrid();
        bool const showScaleBar = d->m_map.showScaleBar();

        d->m_map.setMapThemeId(mapThemeId);

        // Map themes are allowed to change properties. Enforce ours.
        d->m_map.setShowCompass(showCompass);
        d->m_map.setShowOverviewMap(showOverviewMap);
        d->m_map.setShowOtherPlaces(showOtherPlaces);
        d->m_map.setShowGrid(showGrid);
        d->m_map.setShowScaleBar(showScaleBar);

        emit mapThemeIdChanged(mapThemeId);
    }

    void MarbleQuickItem::setShowAtmosphere(bool showAtmosphere)
    {
        if (this->showAtmosphere() == showAtmosphere) {
            return;
        }

        d->m_map.setShowAtmosphere(showAtmosphere);
        emit showAtmosphereChanged(showAtmosphere);
    }

    void MarbleQuickItem::setShowCompass(bool showCompass)
    {
        if (this->showCompass() == showCompass) {
            return;
        }

        d->m_map.setShowCompass(showCompass);
        emit showCompassChanged(showCompass);
    }

    void MarbleQuickItem::setShowClouds(bool showClouds)
    {
        if (this->showClouds() == showClouds) {
            return;
        }

        d->m_map.setShowClouds(showClouds);
        emit showCloudsChanged(showClouds);
    }

    void MarbleQuickItem::setShowCrosshairs(bool showCrosshairs)
    {
        if (this->showCrosshairs() == showCrosshairs) {
            return;
        }

        d->m_map.setShowCrosshairs(showCrosshairs);
        emit showCrosshairsChanged(showCrosshairs);
    }

    void MarbleQuickItem::setShowGrid(bool showGrid)
    {
        if (this->showGrid() == showGrid) {
            return;
        }

        d->m_map.setShowGrid(showGrid);
        emit showGridChanged(showGrid);
    }

    void MarbleQuickItem::setShowOverviewMap(bool showOverviewMap)
    {
        if (this->showOverviewMap() == showOverviewMap) {
            return;
        }

        d->m_map.setShowOverviewMap(showOverviewMap);
        emit showOverviewMapChanged(showOverviewMap);
    }

    void MarbleQuickItem::setShowOtherPlaces(bool showOtherPlaces)
    {
        if (this->showOtherPlaces() == showOtherPlaces) {
            return;
        }

        d->m_map.setShowOtherPlaces(showOtherPlaces);
        emit showOtherPlacesChanged(showOtherPlaces);
    }

    void MarbleQuickItem::setShowScaleBar(bool showScaleBar)
    {
        if (this->showScaleBar() == showScaleBar) {
            return;
        }

        d->m_map.setShowScaleBar(showScaleBar);
        emit showScaleBarChanged(showScaleBar);
    }

    void MarbleQuickItem::setShowBackground(bool showBackground)
    {
        if (this->showBackground() == showBackground) {
            return;
        }

        d->m_map.setShowBackground(showBackground);
        emit showBackgroundChanged(showBackground);
    }

    void MarbleQuickItem::setShowPositionMarker(bool showPositionMarker)
    {
        if (this->showPositionMarker() == showPositionMarker) {
            return;
        }

        QList<RenderPlugin *> plugins = d->m_map.renderPlugins();
        foreach ( RenderPlugin * plugin, plugins ) {
            if (plugin->nameId() == QLatin1String("positionMarker")) {
                plugin->setVisible(showPositionMarker);
                break;
            }
        }

        emit showPositionMarkerChanged(showPositionMarker);
    }

    void MarbleQuickItem::setPositionProvider(const QString &positionProvider)
    {
        QString name;
        if ( d->m_model.positionTracking()->positionProviderPlugin() ) {
            name = d->m_model.positionTracking()->positionProviderPlugin()->nameId();
            if ( name == positionProvider ) {
                return;
            }
        }

        if ( positionProvider.isEmpty() ) {
            d->m_model.positionTracking()->setPositionProviderPlugin( nullptr );
            return;
        }

        QList<const PositionProviderPlugin*> plugins = d->m_model.pluginManager()->positionProviderPlugins();
        foreach (const PositionProviderPlugin* plugin, plugins) {
            if ( plugin->nameId() == positionProvider) {
                PositionProviderPlugin * newPlugin = plugin->newInstance();
                d->m_model.positionTracking()->setPositionProviderPlugin(newPlugin);
                connect(newPlugin, SIGNAL(statusChanged(PositionProviderStatus)), this, SLOT(positionDataStatusChanged(PositionProviderStatus)));
                connect(newPlugin, SIGNAL(positionChanged(GeoDataCoordinates,GeoDataAccuracy)), this, SLOT(updateCurrentPosition(GeoDataCoordinates)));
                connect(newPlugin, SIGNAL(positionChanged(GeoDataCoordinates,GeoDataAccuracy)), this, SIGNAL(speedChanged()));
                connect(newPlugin, SIGNAL(positionChanged(GeoDataCoordinates,GeoDataAccuracy)), this, SIGNAL(angleChanged()));
                emit positionProviderChanged(positionProvider);
                break;
            }
        }
    }

    void MarbleQuickItem::setInertialGlobeRotation(bool inertialGlobeRotation)
    {
        if (inertialGlobeRotation == d->m_inputHandler.inertialEarthRotationEnabled()) {
            return;
        }

        d->m_inputHandler.setInertialEarthRotationEnabled(inertialGlobeRotation);
        emit inertialGlobeRotationChanged(inertialGlobeRotation);
    }

    void MarbleQuickItem::setPluginSetting(const QString &pluginId, const QString &key, const QString &value)
    {
        foreach (RenderPlugin* plugin, d->m_map.renderPlugins()) {
            if (plugin->nameId() == pluginId) {
                plugin->setSetting(key, value);
            }
        }
    }

    void MarbleQuickItem::setPropertyEnabled(const QString &property, bool enabled)
    {
        d->m_map.setPropertyValue(property, enabled);
    }

    bool MarbleQuickItem::isPropertyEnabled(const QString &property) const
    {
        return d->m_map.propertyValue(property);
    }

    void MarbleQuickItem::setShowRuntimeTrace(bool showRuntimeTrace)
    {
        d->m_map.setShowRuntimeTrace(showRuntimeTrace);
        update();
    }

    void MarbleQuickItem::setShowDebugPolygons(bool showDebugPolygons)
    {
        d->m_map.setShowDebugPolygons(showDebugPolygons);
        update();
    }

    void MarbleQuickItem::setShowDebugPlacemarks(bool showDebugPlacemarks)
    {
        d->m_map.setShowDebugPlacemarks(showDebugPlacemarks);
        update();
    }

    void MarbleQuickItem::setShowDebugBatches(bool showDebugBatches)
    {
        d->m_map.setShowDebugBatchRender(showDebugBatches);
        update();
    }

    void MarbleQuickItem::setPlacemarkDelegate(QQmlComponent *placemarkDelegate)
    {
        if (d->m_placemarkDelegate == placemarkDelegate) {
            return;
        }

        delete d->m_placemarkItem;
        d->m_placemarkItem = nullptr;
        d->m_placemarkDelegate = placemarkDelegate;
        emit placemarkDelegateChanged(placemarkDelegate);
    }

    void MarbleQuickItem::loadSettings()
    {
        QSettings settings;
        settings.beginGroup(QStringLiteral("MarbleQuickItem"));
        double lon = settings.value(QStringLiteral("centerLon"), QVariant(0.0)).toDouble();
        double lat = settings.value(QStringLiteral("centerLat"), QVariant(0.0)).toDouble();
        if (lat == 0.0 && lon == 0.0) {
            centerOnCurrentPosition();
        } else {
            centerOn(lon, lat);
        }
        int const zoom = settings.value(QStringLiteral("zoom"), QVariant(0)).toInt();
        if (zoom > 0) {
            setZoom(zoom);
        }
        settings.endGroup();
        d->m_model.routingManager()->readSettings();
        d->m_model.bookmarkManager()->loadFile(QStringLiteral("bookmarks/bookmarks.kml"));
        d->m_model.bookmarkManager()->setShowBookmarks(true);
    }

    void MarbleQuickItem::writeSettings()
    {
        QSettings settings;
        settings.beginGroup(QStringLiteral("MarbleQuickItem"));
        settings.setValue(QStringLiteral("centerLon"), QVariant(d->m_map.centerLongitude()));
        settings.setValue(QStringLiteral("centerLat"), QVariant(d->m_map.centerLatitude()));
        settings.setValue(QStringLiteral("zoom"), QVariant(zoom()));
        settings.endGroup();
        d->m_model.routingManager()->writeSettings();
    }

    QObject *MarbleQuickItem::getEventFilter() const
    {   //We would want to install the same event filter for abstract layer QuickItems such as PinchArea
        return &d->m_inputHandler;
    }

    void MarbleQuickItem::pinch(const QPointF& center, qreal scale, Qt::GestureState state)
    {
        d->m_inputHandler.pinch(center, scale, state);
    }

    MarbleInputHandler *MarbleQuickItem::inputHandler()
    {
        return &d->m_inputHandler;
    }

    int MarbleQuickItem::radius() const
    {
        return d->m_map.radius();
    }

    int MarbleQuickItem::zoom() const
    {
        return d->m_presenter.logzoom();
    }

    bool MarbleQuickItem::layersEventFilter(QObject *, QEvent *)
    {   //Does nothing, but can be reimplemented in a subclass
        return false;
    }
}
