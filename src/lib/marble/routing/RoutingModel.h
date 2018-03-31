//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGMODEL_H
#define MARBLE_ROUTINGMODEL_H

#include "marble_export.h"

#include <QAbstractListModel>

class QIODevice;

/**
  * A QAbstractItemModel that contains a list of routing instructions.
  * Each item represents a routing step in the way from source to
  * destination. Steps near the source come first, steps near the target
  * last.
  */
namespace Marble
{

class RoutingModelPrivate;
class Route;
class RouteRequest;
class GeoDataCoordinates;
class PositionTracking;

class MARBLE_EXPORT RoutingModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( bool deviatedFromRoute READ deviatedFromRoute NOTIFY deviatedFromRoute )

public:
    enum RoutingModelRoles {
        CoordinateRole = Qt::UserRole + 3,
        TurnTypeIconRole,
        LongitudeRole,
        LatitudeRole
    };

    /** Constructor */
    explicit RoutingModel(RouteRequest *request, PositionTracking *positionTracking, QObject *parent = nullptr);

    /** Destructor */
    ~RoutingModel() override;

    // Model querying

    /** Overload of QAbstractListModel */
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const override;

    /** Overload of QAbstractListModel */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

    /** Overload of QAbstractListModel */
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    /** Overload of QAbstractListModel */
    QHash<int, QByteArray> roleNames() const override;

    // Model data filling

    /**
      * Export waypoints and instructions in gpx format
      */
    void exportGpx( QIODevice *device ) const;

    /**
      * Clear any data held in the model
      */
    void clear();

    /**
      * Maps points from the provided route request to waypoints in the model
      * according to their global minimal distance. Returns the right neighbor
      * (next route request item along the waypoints) of the provided position.
      * Provided route must not be null.
      * @return -1 If the provided route is empty, the index of the right
      * neighbor along the waypoints otherwise (result is a valid RouteRequest
      * index in that case)
      */
    int rightNeighbor( const GeoDataCoordinates &position, RouteRequest const *const route ) const;

    /**
     * returns whether the gps location is on route
     */
    bool deviatedFromRoute() const;

    const Route & route() const;

public Q_SLOTS:
    /**
      * Old data in the model is discarded and a model reset is done
      */
    void setRoute( const Route &route );

    void updatePosition( const GeoDataCoordinates&, qreal );

Q_SIGNALS:
   /**
    * emits a signal regarding information about total time( seconds ) and distance( metres ) remaining to reach destination
    */
    void positionChanged();
    void deviatedFromRoute( bool deviated );

    /** A different route was loaded */
    void currentRouteChanged();

private:
    RoutingModelPrivate *const d;
};

} // namespace Marble

#endif
