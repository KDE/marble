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
#include <MarbleModel.h>
#include <MarbleMap.h>
#include <ViewportParams.h>
#include <GeoPainter.h>
#include <GeoDataLookAt.h>
#include <MarbleLocale.h>
#include <Planet.h>
#include <MarbleAbstractPresenter.h>
#include <AbstractFloatItem.h>
#include <MarbleInputHandler.h>

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

    private slots:
        void showLmbMenu(int, int) {}
        void showRmbMenu(int, int) {}
        void openItemToolTip() {}
        void setCursor(const QCursor &cursor)
        {
            m_marbleQuick->setCursor(cursor);
        }

    private slots:
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

    class MarbleQuickItemPrivate : public MarbleAbstractPresenter
    {
    public:
        MarbleQuickItemPrivate(MarbleQuickItem *marble) : MarbleAbstractPresenter()
          ,m_marble(marble)
          ,m_inputHandler(this, marble)
        {
            connect(this, SIGNAL(updateRequired()), m_marble, SLOT(update()));
        }

    private:
        MarbleQuickItem *m_marble;
        friend class MarbleQuickItem;

        MarbleQuickInputHandler m_inputHandler;
    };

    MarbleQuickItem::MarbleQuickItem(QQuickItem *parent) : QQuickPaintedItem(parent)
      ,d(new MarbleQuickItemPrivate(this))
    {
        foreach (AbstractFloatItem *item, d->map()->floatItems())
        {   //TODO: These are not supported in Marble Quick - need refactoring: show them only in
            //MarbleWidget, MarbleQuickItem should not have them accessible
            item->hide();
        }

        connect(d->map(), SIGNAL(repaintNeeded(QRegion)), this, SLOT(update()));
        connect(this, SIGNAL(widthChanged()), this, SLOT(resizeMap()));
        connect(this, SIGNAL(heightChanged()), this, SLOT(resizeMap()));

        setAcceptedMouseButtons(Qt::AllButtons);
        installEventFilter(&d->m_inputHandler);
    }

    void MarbleQuickItem::resizeMap()
    {
        const int minWidth = 100;
        const int minHeight = 100;
        int newWidth = width() > minWidth ? (int)width() : minWidth;
        int newHeight = height() > minHeight ? (int)height() : minHeight;

        d->map()->setSize(newWidth, newHeight);
        update();
    }

    void MarbleQuickItem::paint(QPainter *painter)
    {   //TODO - much to be done here still, i.e paint !enabled version
        QPaintDevice *paintDevice = painter->device();
        QImage image;
        QRect rect = contentsBoundingRect().toRect();

        {
            painter->end();
            GeoPainter geoPainter(paintDevice, d->map()->viewport(), d->map()->mapQuality());
            d->map()->paint(geoPainter, rect);
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
       return d->map()->width();
    }

    int MarbleQuickItem::mapHeight() const
    {
        return d->map()->height();
    }

    bool MarbleQuickItem::showFrameRate() const
    {
        return d->map()->showFrameRate();
    }

    MarbleQuickItem::Projection MarbleQuickItem::projection() const
    {
        return (Projection)d->map()->projection();
    }

    QString MarbleQuickItem::mapThemeId() const
    {
        return d->map()->mapThemeId();
    }

    bool MarbleQuickItem::showAtmosphere() const
    {
        return d->map()->showAtmosphere();
    }

    bool MarbleQuickItem::showCompass() const
    {
        return d->map()->showCompass();
    }

    bool MarbleQuickItem::showClouds() const
    {
        return d->map()->showClouds();
    }

    bool MarbleQuickItem::showCrosshairs() const
    {
        return d->map()->showCrosshairs();
    }

    bool MarbleQuickItem::showGrid() const
    {
        return d->map()->showGrid();
    }

    bool MarbleQuickItem::showOverviewMap() const
    {
        return d->map()->showOverviewMap();
    }

    bool MarbleQuickItem::showOtherPlaces() const
    {
        return d->map()->showOtherPlaces();
    }

    bool MarbleQuickItem::showScaleBar() const
    {
        return d->map()->showScaleBar();
    }

    bool MarbleQuickItem::showBackground() const
    {
        return d->map()->showBackground();
    }

    MarbleModel* MarbleQuickItem::model()
    {
        return d->model();
    }

    const MarbleModel* MarbleQuickItem::model() const
    {
        return d->model();
    }

    MarbleMap* MarbleQuickItem::map()
    {
        return d->map();
    }

    const MarbleMap* MarbleQuickItem::map() const
    {
        return d->map();
    }

    void MarbleQuickItem::setZoom(int newZoom, FlyToMode mode)
    {
        d->setZoom(newZoom, mode);
    }

    void MarbleQuickItem::centerOn(const GeoDataPlacemark& placemark, bool animated)
    {
        d->centerOn(placemark, animated);
    }

    void MarbleQuickItem::centerOn(const GeoDataLatLonBox& box, bool animated)
    {
        d->centerOn(box, animated);
    }

    void MarbleQuickItem::goHome()
    {
        d->goHome();
    }

    void MarbleQuickItem::zoomIn(FlyToMode mode)
    {
        d->zoomIn(mode);
    }

    void MarbleQuickItem::zoomOut(FlyToMode mode)
    {
        d->zoomOut(mode);
    }

    void MarbleQuickItem::setMapWidth(int mapWidth)
    {
        if (d->map()->width() == mapWidth) {
            return;
        }

        d->map()->setSize(mapWidth, mapHeight());
        emit mapWidthChanged(mapWidth);
    }

    void MarbleQuickItem::setMapHeight(int mapHeight)
    {
        if (this->mapHeight() == mapHeight) {
            return;
        }

        d->map()->setSize(mapWidth(), mapHeight);
        emit mapHeightChanged(mapHeight);
    }

    void MarbleQuickItem::setShowFrameRate(bool showFrameRate)
    {
        if (this->showFrameRate() == showFrameRate) {
            return;
        }

        d->map()->setShowFrameRate(showFrameRate);
        emit showFrameRateChanged(showFrameRate);
    }

    void MarbleQuickItem::setProjection(Projection projection)
    {
        if (this->projection() == projection) {
            return;
        }

        d->map()->setProjection((Marble::Projection)projection);
        emit projectionChanged(projection);
    }

    void MarbleQuickItem::setMapThemeId(QString mapThemeId)
    {
        if (this->mapThemeId() == mapThemeId) {
            return;
        }

        d->map()->setMapThemeId(mapThemeId);
        emit mapThemeIdChanged(mapThemeId);
    }

    void MarbleQuickItem::setShowAtmosphere(bool showAtmosphere)
    {
        if (this->showAtmosphere() == showAtmosphere) {
            return;
        }

        d->map()->setShowAtmosphere(showAtmosphere);
        emit showAtmosphereChanged(showAtmosphere);
    }

    void MarbleQuickItem::setShowCompass(bool showCompass)
    {
        if (this->showCompass() == showCompass) {
            return;
        }

        d->map()->setShowCompass(showCompass);
        emit showCompassChanged(showCompass);
    }

    void MarbleQuickItem::setShowClouds(bool showClouds)
    {
        if (this->showClouds() == showClouds) {
            return;
        }

        d->map()->setShowClouds(showClouds);
        emit showCloudsChanged(showClouds);
    }

    void MarbleQuickItem::setShowCrosshairs(bool showCrosshairs)
    {
        if (this->showCrosshairs() == showCrosshairs) {
            return;
        }

        d->map()->setShowCrosshairs(showCrosshairs);
        emit showCrosshairsChanged(showCrosshairs);
    }

    void MarbleQuickItem::setShowGrid(bool showGrid)
    {
        if (this->showGrid() == showGrid) {
            return;
        }

        d->map()->setShowGrid(showGrid);
        emit showGridChanged(showGrid);
    }

    void MarbleQuickItem::setShowOverviewMap(bool showOverviewMap)
    {
        if (this->showOverviewMap() == showOverviewMap) {
            return;
        }

        d->map()->setShowOverviewMap(showOverviewMap);
        emit showOverviewMapChanged(showOverviewMap);
    }

    void MarbleQuickItem::setShowOtherPlaces(bool showOtherPlaces)
    {
        if (this->showOtherPlaces() == showOtherPlaces) {
            return;
        }

        d->map()->setShowOtherPlaces(showOtherPlaces);
        emit showOtherPlacesChanged(showOtherPlaces);
    }

    void MarbleQuickItem::setShowScaleBar(bool showScaleBar)
    {
        if (this->showScaleBar() == showScaleBar) {
            return;
        }

        d->map()->setShowScaleBar(showScaleBar);
        emit showScaleBarChanged(showScaleBar);
    }

    void MarbleQuickItem::setShowBackground(bool showBackground)
    {
        if (this->showBackground() == showBackground) {
            return;
        }

        d->map()->setShowBackground(showBackground);
        emit showBackgroundChanged(showBackground);
    }

    QObject *MarbleQuickItem::getEventFilter() const
    {   //We would want to install the same event filter for abstract layer QuickItems such as PinchArea
        return &d->m_inputHandler;
    }

    void MarbleQuickItem::pinch(QPointF center, qreal scale, Qt::GestureState state)
    {
        d->m_inputHandler.pinch(center, scale, state);
    }

    MarbleInputHandler *MarbleQuickItem::inputHandler()
    {
        return &d->m_inputHandler;
    }

    int MarbleQuickItem::zoom() const
    {
        return d->logzoom();
    }

    bool MarbleQuickItem::layersEventFilter(QObject *, QEvent *)
    {   //Does nothing, but can be reimplemented in a subclass
        return false;
    }
}
