//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTEREQUEST_H
#define MARBLE_ROUTEREQUEST_H

#include "marble_export.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"

#include <QtCore/QFlags>

namespace Marble
{

class RouteRequestPrivate;

/**
  * @brief Points to be included in a route. An ordered list of
  * GeoDataCoordinates with change notification and Pixmap access, similar
  * to QAbstractItemModel.
  */
class MARBLE_EXPORT RouteRequest: public QObject
{
    Q_OBJECT

public:
    enum RoutePreference {
        CarFastest,
        CarShortest,
        Bicycle,
        Pedestrian
    };

    enum AvoidFeature {
        AvoidNone = 0x0,
        AvoidHighway = 0x1,
        AvoidTollWay = 0x2
    };

    Q_DECLARE_FLAGS( AvoidFeatures, AvoidFeature )

    /** Constructor */
    explicit RouteRequest( QObject *parent = 0 );

    /** Destructor */
    ~RouteRequest();

    /** The first point, or a default constructed if empty */
    GeoDataCoordinates source() const;

    /** The last point, or a default constructed if empty */
    GeoDataCoordinates destination() const;

    /** Number of points in the route */
    int size() const;

    /** Accessor for the n-th position */
    GeoDataCoordinates at( int index ) const;

    /** Add the given element to the end */
    void append( const GeoDataCoordinates &coordinates );

    /** Add the given element at the given position */
    void insert( int index, const GeoDataCoordinates &coordinates );

    /** Change the value of the element at the given position */
    void setPosition( int index, const GeoDataCoordinates &position );

    /** Remove the element at the given position */
    void remove( int index );

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
    QPixmap pixmap( int index ) const;

    void setAvoidFeatures( AvoidFeatures features );

    AvoidFeatures avoidFeatures() const;

    void setRoutePreference( RoutePreference preference );

    RoutePreference routePreference() const;

Q_SIGNALS:
    /** The value of the n-th element was changed */
    void positionChanged( int index, const GeoDataCoordinates &position );

    /** An element was added at the given position */
    void positionAdded( int index );

    /** The element at the given position was removed */
    void positionRemoved( int index );

private:
    RouteRequestPrivate *const d;
};

} // namespace Marble

Q_DECLARE_OPERATORS_FOR_FLAGS( Marble::RouteRequest::AvoidFeatures )

#endif
