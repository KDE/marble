// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2019 Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_DECLARATIVE_GEOITEM_H
#define MARBLE_DECLARATIVE_GEOITEM_H

#include "GeoDataCoordinates.h"
#include <QObject>
#include <QQuickItem>
#include <QtQml>

/**
 * Binds a QML item to a specific geodetic location in screen coordinates.
 *
 */
namespace Marble
{
class MarbleQuickItem;

class GeoItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(Marble::MarbleQuickItem *map READ map WRITE setMap NOTIFY mapChanged)

    Q_PROPERTY(qreal longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged)
    Q_PROPERTY(qreal latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(qreal altitude READ altitude WRITE setAltitude NOTIFY altitudeChanged)
    // Determines whether the item is on the visible side of the globe
    Q_PROPERTY(bool observable READ observable NOTIFY observableChanged)
    // We shadow QQuickItem's visible property in order to take the observable into account
    Q_PROPERTY(bool visible READ visObservable WRITE setVisObservable NOTIFY visObservableChanged)

    Q_PROPERTY(qreal x READ readonlyX NOTIFY readonlyXChanged)
    Q_PROPERTY(qreal y READ readonlyY NOTIFY readonlyYChanged)

public:
    /** Constructor */
    explicit GeoItem(QQuickItem *parent = nullptr);

    Q_INVOKABLE bool moveToScreenCoordinates(qreal x, qreal y);

    /** Provides access to the longitude (degree) of the coordinate */
    qreal longitude() const;

    /** Change the longitude of the coordinate */
    void setLongitude(qreal lon);

    /** Provides access to the latitude (degree) of the coordinate */
    qreal latitude() const;

    /** Change the latitude of the coordinate */
    void setLatitude(qreal lat);

    /** Provides access to the altitude (meters) of the coordinate */
    qreal altitude() const;

    /** Change the altitude of the coordinate */
    void setAltitude(qreal alt);

    /** Return all coordinates at once */
    Marble::GeoDataCoordinates coordinates() const;

    /** Change all coordinates at once */
    void setCoordinates(const Marble::GeoDataCoordinates &coordinates);

    /** Query the Marble map backend that this item uses for screen position determination */
    MarbleQuickItem *map() const;

    /** Hook up the GeoItem with Marble's map backend */
    void setMap(MarbleQuickItem *map);

    /** Return whether the item is visible or hidden on the backside of the globe. */
    bool observable() const;

    /** "Shadowed" version for the visible property to take observable into account. */
    bool visObservable() const;
    /** "Shadowed" version for the visible() property to take observable into account. */
    void setVisObservable(bool visible);

    /** "Shadowed" version for the x property to disable writing to the property. */
    qreal readonlyX() const
    {
        return x();
    }

    /** "Shadowed" version for the y property to disable writing to the property. */
    qreal readonlyY() const
    {
        return y();
    }

Q_SIGNALS:
    void longitudeChanged();
    void latitudeChanged();
    void altitudeChanged();

    void mapChanged(MarbleQuickItem *map);

    void observableChanged(bool observable);

    void visObservableChanged(bool visible);

    void readonlyXChanged(qreal x);
    void readonlyYChanged(qreal y);

private:
    Marble::GeoDataCoordinates m_coordinate;
    MarbleQuickItem *m_map = nullptr;
    bool m_observable;
    bool m_visible;
    qreal m_x;
    qreal m_y;

    void updateScreenPosition();
    void setMapToParentOnInit();
};
}

#endif // MARBLE_DECLARATIVE_GEOITEM_H
