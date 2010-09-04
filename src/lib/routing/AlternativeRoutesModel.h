//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ALTERNATIVEROUTESMODEL_H
#define MARBLE_ALTERNATIVEROUTESMODEL_H

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

class AlternativeRoutesModelPrivate;
class RouteRequest;
class MarbleModel;

class AlternativeRoutesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /** Constructor */
    explicit AlternativeRoutesModel( MarbleModel* model, QObject *parent = 0 );

    /** Destructor */
    ~AlternativeRoutesModel();

    // Model querying

    /** Overload of QAbstractListModel */
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const;

    /** Overload of QAbstractListModel */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /** Overload of QAbstractListModel */
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    GeoDataDocument* route( int index );

    // Model data filling

    /** Invalidate the current alternative routes and prepare for new ones to arrive */
    void newRequest( RouteRequest *request );

    /**
      * Old data in the model is discarded, the parsed content of the provided document
      * is used as the new model data and a model reset is done
      */
    void addRoute( GeoDataDocument* document );

    // Convenience methods

    /** Returns the waypoints contained in the route as a linestring */
    static GeoDataLineString* waypoints( const GeoDataDocument* document );

    /** Returns the distance between the given point and the given great circle path */
    static qreal distance( const GeoDataCoordinates &satellite, const GeoDataCoordinates &lineA, const GeoDataCoordinates &lineB );

    /** Returns the minimal distance of each waypoint of routeA to routeB */
    static QVector<qreal> deviation( const GeoDataDocument* routeA, const GeoDataDocument* routeB );

private Q_SLOTS:
    void addRestrainedRoutes();

    void update( GeoDataDocument* route );

private:
    AlternativeRoutesModelPrivate *const d;
};

} // namespace Marble

#endif
