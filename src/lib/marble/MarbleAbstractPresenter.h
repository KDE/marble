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

#include <QList>

#include "GeoDataLatLonBox.h"
#include "MarblePhysics.h"
#include <marble_export.h>

namespace Marble
{

class GeoDataPlacemark;
class GeoDataLookAt;
class MarbleMap;
class MarbleModel;
class ViewportParams;

    class MARBLE_EXPORT MarbleAbstractPresenter : public QObject
    {
    Q_OBJECT

    Q_SIGNALS:
        void zoomChanged(int zoom);
        void distanceChanged(const QString& distanceString);

        /** This signal is emitted when a new rectangle region is selected over the map
        *  The list of double values includes coordinates in degrees using the following:
        *  lon1, lat1, lon2, lat2 (or West, North, East, South) as left/top, right/bottom rectangle.
        */
        void regionSelected(const QList<double>&);

    public:
        explicit MarbleAbstractPresenter(MarbleMap *map, QObject *parent = 0);
        virtual ~MarbleAbstractPresenter();

        qreal moveStep() const;
        int radius() const;

        GeoDataLookAt lookAt() const;

        QString distanceString() const;

        /**
         * @brief Approximated altitude of the camera in km
         */
        qreal distance() const;

        /**
         * @brief An approximate distance from @p radius
         * @param radius radius of planet disc in screen pixels
         */
        qreal distanceFromRadius(qreal radius) const;

        /**
         * @brief The radius of the rendered planet disc derived from the approximate apparent @p distance
         */
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

    public Q_SLOTS:
        void rotateBy(const qreal deltaLon, const qreal deltaLat, FlyToMode mode = Instant);
        void flyTo(const GeoDataLookAt &newLookAt, FlyToMode mode = Automatic);
        void goHome(FlyToMode mode = Automatic);

        void setZoom(int newZoom, FlyToMode mode = Instant);
        void zoomView(int zoom, FlyToMode mode = Instant);
        void zoomViewBy(int zoomStep, FlyToMode mode = Instant);
        void zoomIn(FlyToMode mode = Automatic);
        void zoomOut(FlyToMode mode = Automatic);
        void zoomAtBy(const QPoint &pos, int zoomStep);

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
        MarbleMap *const m_map;
        MarblePhysics m_physics;

        bool m_animationsEnabled;
        int m_logzoom;
        int m_zoomStep;
        const qreal m_viewAngle;
    };
}


#endif // MARBLEABSTRACTPRESENTER_H
