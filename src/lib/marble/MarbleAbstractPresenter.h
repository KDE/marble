//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#ifndef MARBLEABSTRACTPRESENTER_H
#define MARBLEABSTRACTPRESENTER_H

#include <QSharedPointer>
#include <QList>
#include "GeoDataLookAt.h"
#include "GeoDataLatLonBox.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarblePhysics.h"

namespace Marble
{
    class MarbleAbstractPresenter : public QObject
    {
    Q_OBJECT

    signals:
        void zoomChanged(int zoom);
        void distanceChanged(const QString& distanceString);
        void updateRequired();

        /** This signal is emitted when a new rectangle region is selected over the map
        *  The list of double values includes coordinates in degrees using the following:
        *  lon1, lat1, lon2, lat2 (or West, North, East, South) as left/top, right/bottom rectangle.
        */
        void regionSelected(const QList<double>&);

    public:
        MarbleAbstractPresenter();
        void initialize(MarbleModel *model, MarbleMap *map);
        virtual ~MarbleAbstractPresenter();

        qreal moveStep() const;
        int radius() const;

        GeoDataLookAt lookAt() const;
        qreal distance() const;
        qreal distanceFromRadius(qreal radius) const;
        QString distanceString() const;
        qreal radiusFromDistance(qreal distance) const;

        /**
          * @brief Rotate the globe in the given direction in discrete steps
          * @param stepsRight Number of steps to go right. Negative values go left.
          * @param stepsDown Number of steps to go down. Negative values go up.
          * @param mode Interpolation mode to use when traveling to the target
          */
        void moveByStep(int stepsRight, int stepsDown, FlyToMode mode = Automatic);

        int polarity() const;
        int zoom() const;
        int minimumZoom() const;
        int maximumZoom() const;

        qreal distanceFromZoom(qreal zoom) const;
        qreal zoomFromDistance(qreal distance) const;

        void zoomAt(const QPoint &pos, qreal newDistance);
        void moveTo(const QPoint &pos, qreal factor);

        qreal centerLongitude() const;
        qreal centerLatitude() const;

        ViewContext viewContext() const;

        qreal zoom(qreal radius) const;
        qreal radius(qreal zoom) const;

        MarbleMap* map();
        MarbleModel* model();

        const MarbleMap* map() const;
        const MarbleModel* model() const;

        int logzoom() const;
        void setLogzoom(int value);

        int zoomStep() const;
        qreal viewAngle() const;

        bool animationsEnabled() const;

        ViewportParams *viewport();
        const ViewportParams* viewport() const;

    public slots:
        void rotateBy(const qreal deltaLon, const qreal deltaLat, FlyToMode mode = Instant);
        void flyTo(const GeoDataLookAt &newLookAt, FlyToMode mode = Automatic);
        void goHome(FlyToMode mode = Automatic);

        void setZoom(int newZoom, FlyToMode mode = Instant);
        void zoomView(int zoom, FlyToMode mode = Instant);
        void zoomViewBy(int zoomStep, FlyToMode mode = Instant);
        void zoomIn(FlyToMode mode = Automatic);
        void zoomOut(FlyToMode mode = Automatic);

        void setViewContext(ViewContext viewContext);

        void centerOn(const qreal lon, const qreal lat, bool animated = false);
        void centerOn(const GeoDataCoordinates &point, bool animated = false);
        void centerOn(const GeoDataLatLonBox& box, bool animated = false);
        void centerOn(const GeoDataPlacemark& placemark, bool animated = false);
        void setCenterLatitude(qreal lat, FlyToMode mode);
        void setCenterLongitude(qreal lon, FlyToMode mode);

        void setAnimationsEnabled(bool enabled);
        void setRadius(int radius);
        void setDistance(qreal newDistance);
        void setSelection(const QRect& region);

    private:
        //MarbleAbstractPresenter owns these
        MarbleModel m_model;
        MarbleMap m_map;
        MarblePhysics m_physics;

        bool m_animationsEnabled;
        int m_logzoom;
        int m_zoomStep;
        const qreal m_viewAngle;
    };
}


#endif // MARBLEABSTRACTPRESENTER_H
