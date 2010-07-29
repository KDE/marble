//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTINGMODEL_H
#define MARBLE_ROUTINGMODEL_H

#include "GeoDataLineString.h"
#include "MarblePlacemarkModel.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QTime>

/**
  * A QAbstractItemModel that contains a list of routing instructions.
  * Each item represents a routing step in the way from source to
  * destination. Steps near the source come first, steps near the target
  * last.
  */
namespace Marble
{

class RoutingModelPrivate;
class RouteSkeleton;
class GeoDataDocument;
class MarbleModel;
class RoutingModel : public QAbstractListModel
{
    Q_OBJECT

public:
    struct Duration
    {
        unsigned int days;
        QTime time;

        explicit Duration( unsigned int days_ = 0, const QTime &time_ = QTime() )
            :days(days_), time(time_)
        {
            // nothing to do
        }
    };

    enum RoutingItemDataRole {
        CoordinateRole = MarblePlacemarkModel::CoordinateRole, // synchronized with MarblePlacemarkModel
        TypeRole = CoordinateRole + 24 ,// avoid conflict with MarblePlacemarkModel
        InstructionWayPointRole = TypeRole + 1,
    };

    enum RoutingItemType {
        WayPoint,
        Instruction,
        Error
    };

    /** Constructor */
    explicit RoutingModel( MarbleModel *model, QObject *parent = 0 );

    /** Destructor */
    ~RoutingModel();

    // Model querying

    /** Overload of QAbstractListModel */
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const;

    /** Overload of QAbstractListModel */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /** Overload of QAbstractListModel */
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    // Model data filling

    // importGeoDataDocument() will probably become setModel(GeoDataDocument*) in the future and
    // the internal representation using a QVector<RouteElement> may go away
    /**
      * Old data in the model is discarded, the parsed content of the provided document
      * is used as the new model data and a model reset is done
      */
    bool importGeoDataDocument( GeoDataDocument* document );

    /**
      * Returns the total (estimated) time it takes to travel from
-     * source to destination
      */
    Duration duration() const;

    /**
      * Returns the total route distance ( meters )
      */
    qreal totalDistance() const;

    /**
      * Export waypoints and instructions in gpx format
      */
    void exportGpx( QIODevice *device ) const;

    /**
      * Clear any data held in the model
      */
    void clear();

    /**
      * Maps points from the provided route skeleton to waypoints in the model
      * according to their global minimal distance. Returns the right neighbor
      * (next route skeleton item along the waypoints) of the provided position.
      * Provided route must not be null.
      * @return -1 If the provided route is empty, the index of the right
      * neighbor along the waypoints otherwise (result is a valid RouteSkeleton
      * index in that case)
      */
    int rightNeighbor( const GeoDataCoordinates &position, RouteSkeleton const *const route ) const;

    /**
      * returns the time remaining( minutes ) to reach the next instruction point
      */
    qreal remainingTime() const;

    /**
      * returns index of the next instruction to be shown
      */
    int showInstructionIndex() const;

    /**
      * returns the position of next instruction point
      */
    GeoDataCoordinates instructionPoint() const;

    /**
      * returns the next valid instruction
      */
    QString instructionText() const;

    /**
      * returns the total time remaining to reach the destination ( minutes )
      */
    qreal totalTimeRemaining() const;

    ///returns whether the gpsLocation is on route
    bool deviatedFromRoute() const;

public Q_SLOTS:
    void currentInstruction( GeoDataCoordinates, qreal );

Q_SIGNALS:
    /**
     * emits a signal regarding information about total time( minutes ) and distance( metres ) remaining to reach destination
     */
    void nextInstruction( qreal totalTimeRemaining, qreal totalDistanceRemaining);
    void routeCalculated( int );

private:
    RoutingModelPrivate *const d;
};

} // namespace Marble

Q_DECLARE_METATYPE( Marble::RoutingModel::RoutingItemType )

#endif
