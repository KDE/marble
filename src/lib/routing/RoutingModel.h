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
        TypeRole = CoordinateRole + 24 // avoid conflict with MarblePlacemarkModel
    };

    enum RoutingItemType {
        WayPoint,
        Instruction,
        Error
    };

    /** Constructor */
    explicit RoutingModel( QObject *parent = 0 );

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

    /**
      * @brief Parse the provided data in opengis xml format.
      *
      * Old data in the model is discarded, the parsed content of the provided xml data
      * in opengis format is used as the new model data and a model reset is done
      * @param xmlData Route instructions, xml. See document 03-006r3:
      *  "OpenGIS Location Services (OpenLS): Core Services [Parts 5 Route Service]" at
      *  http://www.opengeospatial.org/standards/ols
      * @param route The route points (ordered, first is source, last is destination)
      * that were used to generate the xml data
      *
      * @todo: Add geodata/handlers/opengis, use it in the route provider and use a
      * parsed GeoDocument here
      */
    void importOpenGis( const QByteArray &xmlData );

    /**
      * Returns the total (estimated) time it takes to travel from
-     * source to destination
      */
    Duration duration() const;

    /**
      * Returns the total route distance (kilometer)
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

private:
    RoutingModelPrivate *const d;
};

} // namespace Marble

Q_DECLARE_METATYPE( Marble::RoutingModel::RoutingItemType )

#endif
