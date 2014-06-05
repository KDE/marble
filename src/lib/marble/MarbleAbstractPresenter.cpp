//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#include <MarbleAbstractPresenter.h>
#if QT_VERSION >= 0x050000
    #include <QtMath>
#else
    #include <qmath.h>
#endif
#include <Quaternion.h>
#include <ViewportParams.h>
#include <MarbleLocale.h>
#include <Planet.h>
#include <GeoDataPlacemark.h>
#include <MarbleClock.h>
#include <MarbleDebug.h>

namespace Marble
{
    MarbleAbstractPresenter::MarbleAbstractPresenter() :
        QObject()
        ,m_model()
        ,m_map(&m_model)
        ,m_physics(this)
        ,m_animationsEnabled(false)
        ,m_logzoom(0)
        ,m_zoomStep(MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 60 : 40)
        ,m_viewAngle(110)
    {
    }

    MarbleAbstractPresenter::~MarbleAbstractPresenter()
    {
    }

    qreal MarbleAbstractPresenter::zoom(qreal radius) const
    {
        return (200.0 * log(radius));
    }

    qreal MarbleAbstractPresenter::radius(qreal zoom) const
    {
        return pow(M_E, (zoom / 200.0));
    }

    void MarbleAbstractPresenter::rotateBy(const qreal deltaLon, const qreal deltaLat, FlyToMode mode)
    {
        Quaternion rotPhi(1.0, deltaLat / 180.0, 0.0, 0.0);
        Quaternion rotTheta(1.0, 0.0, deltaLon / 180.0, 0.0);

        Quaternion axis = map()->viewport()->planetAxis();
        qreal lon(0.0), lat(0.0);
        axis.getSpherical(lon, lat);
        axis = rotTheta * axis;
        axis *= rotPhi;
        axis.normalize();
        lat = -axis.pitch();
        lon = axis.yaw();

        GeoDataLookAt target = lookAt();
        target.setLongitude(lon);
        target.setLatitude(lat);
        flyTo(target, mode);
    }

    void MarbleAbstractPresenter::flyTo(const GeoDataLookAt &newLookAt, FlyToMode mode)
    {
        if (!m_animationsEnabled || mode == Instant)
        {
            const int radius = qRound(radiusFromDistance(newLookAt.range() * METER2KM));
            qreal const zoomVal = zoom(radius);

            // Prevent exceeding zoom range. Note: Bounding to range is not useful here
            if (qRound(zoomVal) >= minimumZoom() && qRound(zoomVal) <= maximumZoom())
            {
                map()->setRadius(radius);
                m_logzoom = qRound(zoom(radius));

                GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
                map()->centerOn(newLookAt.longitude(deg), newLookAt.latitude(deg));

                emit zoomChanged(m_logzoom);
                emit distanceChanged(distanceString());

                emit updateRequired();
            }
        }
        else
        {
            m_physics.flyTo(newLookAt, mode);
        }
    }

    QString MarbleAbstractPresenter::distanceString() const
    {
        qreal dist = distance();
        QString distanceUnitString;

        const MarbleLocale::MeasurementSystem measurementSystem =
                MarbleGlobal::getInstance()->locale()->measurementSystem();

        switch (measurementSystem)
        {
        case MarbleLocale::MetricSystem:
            distanceUnitString = tr("km");
            break;
        case MarbleLocale::ImperialSystem:
            dist *= KM2MI;
            distanceUnitString = tr("mi");
            break;
        case MarbleLocale::NauticalSystem:
            dist *= KM2NM;
            distanceUnitString = tr("nm");
            break;
        }

        return QString("%L1 %2").arg(dist, 8, 'f', 1, QChar(' ')).arg(distanceUnitString);
    }

    GeoDataLookAt MarbleAbstractPresenter::lookAt() const
    {
        GeoDataLookAt result;

        result.setLongitude(map()->viewport()->centerLongitude());
        result.setLatitude(map()->viewport()->centerLatitude());
        result.setAltitude(0.0);
        result.setRange(distance() * KM2METER);

        return result;
    }

    qreal MarbleAbstractPresenter::distance() const
    {
        return distanceFromRadius(radius());
    }

    qreal MarbleAbstractPresenter::distanceFromRadius(qreal radius) const
    {
        // Due to Marble's orthographic projection ("we have no focus")
        // it's actually not possible to calculate a "real" distance.
        // Additionally the viewing angle of the earth doesn't adjust to
        // the window's size.
        //
        // So the only possible workaround is to come up with a distance
        // definition which gives a reasonable approximation of
        // reality. Therefore we assume that the average window width
        // (about 800 pixels) equals the viewing angle of a human being.

        return (model()->planet()->radius() * 0.4
                / radius / tan(0.5 * m_viewAngle * DEG2RAD));
    }

    qreal MarbleAbstractPresenter::radiusFromDistance(qreal distance) const
    {
        return model()->planet()->radius() /
                (distance * tan(0.5 * m_viewAngle * DEG2RAD) / 0.4 );
    }

    int MarbleAbstractPresenter::polarity() const
    {
        return map()->viewport()->polarity();
    }

    int MarbleAbstractPresenter::zoom() const
    {
        return m_logzoom;
    }

    int MarbleAbstractPresenter::minimumZoom() const
    {
        return map()->minimumZoom();
    }

    int MarbleAbstractPresenter::maximumZoom() const
    {
        return map()->maximumZoom();
    }

    void MarbleAbstractPresenter::setZoom(int newZoom, FlyToMode mode)
    {
        // It won't fly anyway. So we should do everything to keep the zoom value.
        if (!m_animationsEnabled || mode == Instant)
        {
            // Check for under and overflow.
            if (newZoom < minimumZoom())
                newZoom = minimumZoom();
            else if (newZoom > maximumZoom())
                newZoom = maximumZoom();

            // Prevent infinite loops.
            if (newZoom == m_logzoom)
                return;

            map()->setRadius(radius(newZoom));
            m_logzoom = newZoom;

            emit zoomChanged(m_logzoom);
            emit distanceChanged(distanceString());
            emit updateRequired();
        }
        else
        {
            GeoDataLookAt target = lookAt();
            target.setRange(KM2METER * distanceFromZoom(newZoom));
            flyTo(target, mode);
        }
    }

    void MarbleAbstractPresenter::zoomView(int zoom, FlyToMode mode)
    {
        setZoom(zoom, mode);
    }

    void MarbleAbstractPresenter::zoomViewBy(int zoomStep, FlyToMode mode)
    {
        setZoom(zoom() + zoomStep, mode);
    }

    void MarbleAbstractPresenter::zoomIn(FlyToMode mode)
    {
        if (map()->tileZoomLevel() < 0)
        {
            zoomViewBy(m_zoomStep, mode);
        }
        else
        {
            int radiusVal = map()->preferredRadiusCeil(map()->radius() * 1.05);
            radiusVal = qMax<int>(radius(minimumZoom()), qMin<int>(radiusVal, radius(maximumZoom())));

            GeoDataLookAt target = lookAt();
            target.setRange(KM2METER * distanceFromRadius(radiusVal));

            flyTo(target, mode);
        }
    }

    void MarbleAbstractPresenter::zoomOut(FlyToMode mode)
    {
        if (map()->tileZoomLevel() <= 0)
        {
            zoomViewBy(-m_zoomStep, mode);
        }
        else
        {
            int radiusVal = map()->preferredRadiusFloor(map()->radius() * 0.95);
            radiusVal = qMax<int>(radius(minimumZoom()), qMin<int>(radiusVal, radius(maximumZoom())));

            GeoDataLookAt target = lookAt();
            target.setRange(KM2METER * distanceFromRadius(radiusVal));

            flyTo(target, mode);
        }
    }

    qreal MarbleAbstractPresenter::distanceFromZoom(qreal zoom) const
    {
        return distanceFromRadius(radius(zoom));
    }

    qreal MarbleAbstractPresenter::zoomFromDistance(qreal distance) const
    {
        return zoom(radiusFromDistance(distance));
    }

    void MarbleAbstractPresenter::goHome(FlyToMode mode)
    {
        qreal homeLon = 0;
        qreal homeLat = 0;
        int homeZoom = 0;
        model()->home(homeLon, homeLat, homeZoom);

        GeoDataLookAt target;
        target.setLongitude(homeLon, GeoDataCoordinates::Degree);
        target.setLatitude(homeLat, GeoDataCoordinates::Degree);
        target.setRange(1000 * distanceFromZoom(homeZoom));

        flyTo(target, mode);
    }

    void MarbleAbstractPresenter::moveByStep(int stepsRight, int stepsDown, FlyToMode mode)
    {
        int polarity = map()->viewport()->polarity();
        qreal left = polarity * stepsRight * moveStep();
        qreal down = stepsDown * moveStep();
        rotateBy(left, down, mode);
    }

    qreal MarbleAbstractPresenter::moveStep() const
    {
        int width = map()->width();
        int height = map()->height();

        if (radius() < qSqrt((qreal)(width * width + height * height)))
            return 180.0 * 0.1;
        else
            return 180.0 * qAtan((qreal)width
                         / (qreal)(2 * radius())) * 0.2;
    }

    int MarbleAbstractPresenter::radius() const
    {
        return map()->radius();
    }

    void MarbleAbstractPresenter::setRadius(int radiusVal)
    {
        Q_ASSERT(radiusVal >= 0);
        bool adjustRadius = radiusVal != map()->radius();

        qreal const zoomVal = zoom(radiusVal);

        // Prevent exceeding zoom range
        if (zoomVal < minimumZoom())
        {
            radiusVal = radius(minimumZoom());
            adjustRadius = true;
        }
        else if (zoomVal > maximumZoom())
        {
            radiusVal = radius(maximumZoom());
            adjustRadius = true;
        }

        if (adjustRadius)
        {
            map()->setRadius(radiusVal);
            m_logzoom = qRound(zoomVal);

            emit zoomChanged(m_logzoom);
            emit distanceChanged(distanceString());
            emit updateRequired();
        }
    }


    //Moved from MarbleWidgetInputHandlerPrivate - fits more here now
    void MarbleAbstractPresenter::zoomAt(const QPoint &pos, qreal newDistance)
    {
        Q_ASSERT(newDistance > 0.0);

        qreal destLat;
        qreal destLon;
        if (!map()->geoCoordinates(pos.x(), pos.y(), destLon, destLat, GeoDataCoordinates::Degree))
        {
            return;
        }

        ViewportParams* now = map()->viewport();
        qreal x(0), y(0);
        if (!now->screenCoordinates(destLon * DEG2RAD, destLat * DEG2RAD, x, y))
        {
            return;
        }

        ViewportParams soon;
        soon.setProjection(now->projection());
        soon.centerOn(now->centerLongitude(), now->centerLatitude());
        soon.setSize(now->size());

        qreal newRadius = radiusFromDistance(newDistance);
        soon.setRadius(newRadius);

        qreal mouseLon, mouseLat;
        if (!soon.geoCoordinates(int(x), int(y), mouseLon, mouseLat, GeoDataCoordinates::Degree ))
        {
            return;
        }

        const qreal lon = destLon - (mouseLon - map()->centerLongitude());
        const qreal lat = destLat - (mouseLat - map()->centerLatitude());

        GeoDataLookAt lookAt;
        lookAt.setLongitude(lon, GeoDataCoordinates::Degree);
        lookAt.setLatitude(lat, GeoDataCoordinates::Degree);
        lookAt.setAltitude(0.0);
        lookAt.setRange(newDistance * KM2METER);

        map()->viewport()->setFocusPoint(GeoDataCoordinates(destLon, destLat, 0, GeoDataCoordinates::Degree));
        flyTo(lookAt, Linear);
    }

    void MarbleAbstractPresenter::moveTo(const QPoint &pos, qreal factor)
    {
        Q_ASSERT(factor > 0.0);

        qreal destLat;
        qreal destLon;
        map()->geoCoordinates(pos.x(), pos.y(), destLon, destLat, GeoDataCoordinates::Radian);

        GeoDataLookAt lookAt;
        lookAt.setLongitude(destLon);
        lookAt.setLatitude(destLat);
        lookAt.setAltitude(0.0);
        lookAt.setRange(distance() * factor * KM2METER);

        flyTo(lookAt);
    }

    void MarbleAbstractPresenter::centerOn(const qreal lon, const qreal lat, bool animated)
    {
        GeoDataCoordinates target(lon, lat, 0.0, GeoDataCoordinates::Degree);
        centerOn(target, animated);
    }

    void MarbleAbstractPresenter::centerOn(const GeoDataCoordinates &position, bool animated)
    {
        GeoDataLookAt target = lookAt();
        target.setCoordinates(position);
        flyTo(target, animated ? Automatic : Instant);
    }

    void MarbleAbstractPresenter::centerOn(const GeoDataLatLonBox &box, bool animated)
    {
        if (box.isEmpty())
        {
            return;
        }

        int newRadius = radius();
        ViewportParams* viewparams = map()->viewport();
        //prevent divide by zero
        if(box.height() && box.width())
        {
            //work out the needed zoom level
            int const horizontalRadius = ( 0.25 * M_PI ) * (viewparams->height() / box.height());
            int const verticalRadius = ( 0.25 * M_PI ) * (viewparams->width() / box.width());
            newRadius = qMin<int>(horizontalRadius, verticalRadius );
            newRadius = qMax<int>(radius(minimumZoom()), qMin<int>(newRadius, radius(maximumZoom())));
        }

        //move the map
        GeoDataLookAt target;
        target.setCoordinates(box.center());
        target.setAltitude(box.center().altitude());
        target.setRange(KM2METER * distanceFromRadius(newRadius));
        flyTo(target, animated ? Automatic : Instant);
    }

    void MarbleAbstractPresenter::centerOn(const GeoDataPlacemark& placemark, bool animated)
    {
        const GeoDataLookAt *lookAt(placemark.lookAt());
        if (lookAt)
        {
            flyTo(*lookAt, animated ? Automatic : Instant);
        }
        else
        {
            bool icon;
            GeoDataCoordinates coords = placemark.coordinate(model()->clock()->dateTime(), &icon);
            if (icon)
            {
                centerOn(coords, animated);
            }
            else
            {
                centerOn(placemark.geometry()->latLonAltBox(), animated);
            }
        }
    }

    void MarbleAbstractPresenter::setCenterLatitude(qreal lat, FlyToMode mode)
    {
        centerOn(centerLongitude(), lat, mode);
    }

    void MarbleAbstractPresenter::setCenterLongitude(qreal lon, FlyToMode mode)
    {
        centerOn(lon, centerLatitude(), mode);
    }

    qreal MarbleAbstractPresenter::centerLatitude() const
    {
        return map()->centerLatitude();
    }

    qreal MarbleAbstractPresenter::centerLongitude() const
    {
        return map()->centerLongitude();
    }

    ViewContext MarbleAbstractPresenter::viewContext() const
    {
        return map()->viewContext();
    }

    void MarbleAbstractPresenter::setViewContext(ViewContext viewContext)
    {
        if (map()->viewContext() != viewContext)
        {
            const MapQuality oldQuality = map()->mapQuality();
            map()->setViewContext(viewContext);

            //TODO - set view context for routing layer
            //m_routingLayer->setViewContext( viewContext );

            if (map()->mapQuality() != oldQuality)
            {
                emit updateRequired();
            }
        }
    }

    bool MarbleAbstractPresenter::animationsEnabled() const
    {
        return m_animationsEnabled;
    }

    void MarbleAbstractPresenter::setAnimationsEnabled(bool enabled)
    {
        m_animationsEnabled = enabled;
    }

    int MarbleAbstractPresenter::logzoom() const
    {
        return m_logzoom;
    }

    void MarbleAbstractPresenter::setLogzoom(int value)
    {
        m_logzoom = value;
    }

    int MarbleAbstractPresenter::zoomStep() const
    {
        return m_zoomStep;
    }

    qreal MarbleAbstractPresenter::viewAngle() const
    {
        return m_viewAngle;
    }

    MarbleMap* MarbleAbstractPresenter::map()
    {
        return &m_map;
    }

    const MarbleMap* MarbleAbstractPresenter::map() const
    {
        return &m_map;
    }

    MarbleModel* MarbleAbstractPresenter::model()
    {
        return &m_model;
    }

    const MarbleModel* MarbleAbstractPresenter::model() const
    {
        return &m_model;
    }

    ViewportParams* MarbleAbstractPresenter::viewport()
    {
        return map()->viewport();
    }

    const ViewportParams* MarbleAbstractPresenter::viewport() const
    {
        return map()->viewport();
    }

    void MarbleAbstractPresenter::setDistance(qreal newDistance)
    {
        qreal minDistance = 0.001;

        if (newDistance <= minDistance)
        {
            mDebug() << "Invalid distance: 0 m";
            newDistance = minDistance;
        }

        int newRadius = radiusFromDistance(newDistance);
        setRadius(newRadius);
    }

    void MarbleAbstractPresenter::setSelection(const QRect& region)
    {
        QPoint tl = region.topLeft();
        QPoint br = region.bottomRight();
        mDebug() << "Selection region: (" << tl.x() << ", " <<  tl.y() << ") ("
            << br.x() << ", " << br.y() << ")" << endl;

         GeoDataLatLonAltBox box = viewport()->latLonAltBox(region);

         // NOTE: coordinates as lon1, lat1, lon2, lat2 (or West, North, East, South)
         // as left/top, right/bottom rectangle.
         QList<double> coordinates;
         coordinates << box.west(GeoDataCoordinates::Degree) << box.north(GeoDataCoordinates::Degree)
                     << box.east(GeoDataCoordinates::Degree) << box.south(GeoDataCoordinates::Degree);

         mDebug() << "West: " << coordinates[0] << " North: " <<  coordinates[1]
                  << " East: " << coordinates[2] << " South: " << coordinates[3] << endl;

         emit regionSelected(coordinates);
     }

}

#include "MarbleAbstractPresenter.moc"

