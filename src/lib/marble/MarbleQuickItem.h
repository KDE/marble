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

#include <QSharedPointer>
#include <QQuickPaintedItem>
#include "GeoDataPlacemark.h"
#include "MarbleGlobal.h"

namespace Marble
{
    class MarbleModel;
    class MarbleMap;
    class MarbleInputHandler;
    class MarbleQuickItemPrivate;

    //Class is still being developed
    class MARBLE_EXPORT MarbleQuickItem : public QQuickPaintedItem
    {
    Q_OBJECT

        Q_ENUMS(Projection)

        Q_PROPERTY(int mapWidth READ mapWidth WRITE setMapWidth NOTIFY mapWidthChanged)
        Q_PROPERTY(int mapHeight READ mapHeight WRITE setMapHeight NOTIFY mapHeightChanged)
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

    public:
        MarbleQuickItem(QQuickItem *parent = 0);

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

    public slots:
        void goHome();
        void setZoom(int zoom, FlyToMode mode = Instant);
        void centerOn(const GeoDataPlacemark& placemark, bool animated = false);
        void centerOn(const GeoDataLatLonBox& box, bool animated = false);

        void zoomIn(FlyToMode mode = Automatic);
        void zoomOut(FlyToMode mode = Automatic);

        void setMapWidth(int mapWidth);
        void setMapHeight(int mapHeight);
        void setShowFrameRate(bool showFrameRate);
        void setProjection(Projection projection);
        void setMapThemeId(QString mapThemeId);
        void setShowAtmosphere(bool showAtmosphere);
        void setShowCompass(bool showCompass);
        void setShowClouds(bool showClouds);
        void setShowCrosshairs(bool showCrosshairs);
        void setShowGrid(bool showGrid);
        void setShowOverviewMap(bool showOverviewMap);
        void setShowOtherPlaces(bool showOtherPlaces);
        void setShowScaleBar(bool showScaleBar);
        void setShowBackground(bool showBackground);

    // QQuickPaintedItem interface
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

    signals:
        void mapWidthChanged(int mapWidth);
        void mapHeightChanged(int mapHeight);
        void showFrameRateChanged(bool showFrameRate);
        void projectionChanged(Projection projection);
        void mapThemeIdChanged(QString mapThemeId);
        void showAtmosphereChanged(bool showAtmosphere);
        void showCompassChanged(bool showCompass);
        void showCloudsChanged(bool showClouds);
        void showCrosshairsChanged(bool showCrosshairs);
        void showGridChanged(bool showGrid);
        void showOverviewMapChanged(bool showOverviewMap);
        void showOtherPlacesChanged(bool showOtherPlaces);
        void showScaleBarChanged(bool showScaleBar);
        void showBackgroundChanged(bool showBackground);

    protected:
        MarbleModel* model();
        const MarbleModel* model() const;

        MarbleMap* map();
        const MarbleMap* map() const;

        QObject *getEventFilter() const;
        void pinch(QPointF center, qreal scale, Qt::GestureState state);

    private slots:
        void resizeMap();

    private:
        typedef QSharedPointer<MarbleQuickItemPrivate> MarbleQuickItemPrivatePtr;
        MarbleQuickItemPrivatePtr d;
        friend class MarbleQuickItemPrivate;
    };
}

#endif // MARBLEQUICKITEM_H
