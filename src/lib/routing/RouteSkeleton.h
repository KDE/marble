/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARBLE_ROUTE_SKELETON_H
#define MARBLE_ROUTE_SKELETON_H

#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"

namespace Marble {

class RouteSkeletonPrivate;

/**
  * @brief Points to be included in a route. An ordered list of
  * GeoDataCoordinates with change notification and Pixmap access, similar
  * to QAbstractItemModel.
  */
class RouteSkeleton: public QObject
{
    Q_OBJECT

public:
    /** Constructor */
    explicit RouteSkeleton(QObject *parent = 0);

    /** Destructor */
    ~RouteSkeleton();

    /** The first point, or a default constructed if empty */
    GeoDataCoordinates source() const;

    /** The last point, or a default constructed if empty */
    GeoDataCoordinates destination() const;

    /** Number of points in the route */
    int size() const;

    /** Accessor for the n-th position */
    GeoDataCoordinates at(int index) const;

    /** Add the given element to the end */
    void append(const GeoDataCoordinates &coordinates);

    /** Add the given element at the given position */
    void insert(int index, const GeoDataCoordinates &coordinates);

    /** Change the value of the element at the given position */
    void setPosition( int index, const GeoDataCoordinates &position);

    /** Remove the element at the given position */
    void remove(int index);

    /** Remove all elements */
    void clear();

    /**
      * Insert a via point. Order will be chosen such that the via point is not before
      * the start or after the destination. Furthermore the distance between neighboring
      * route points is minimized
      *
      * @note: This does not trigger an update of the route. It becomes "dirty"
      *
      * @todo: Minimizing the distance might not always be what the user wants
      */
    void addVia( const GeoDataCoordinates &position );

    /** Returns a pixmap which indicates the position of the element */
    QPixmap pixmap(int index) const;

Q_SIGNALS:
    /** The value of the n-th element was changed */
    void positionChanged(int index, const GeoDataCoordinates &position);

    /** An element was added at the given position */
    void positionAdded(int index);

private:
    RouteSkeletonPrivate* const d;
};

} // namespace Marble

#endif // MARBLE_ROUTE_SKELETON_H
