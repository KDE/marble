//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ALTERNATIVEROUTESMODEL_H
#define MARBLE_ALTERNATIVEROUTESMODEL_H

#include "marble_export.h"

#include <QAbstractListModel>

/**
  * A QAbstractItemModel that contains a list of routing instructions.
  * Each item represents a routing step in the way from source to
  * destination. Steps near the source come first, steps near the target
  * last.
  */
namespace Marble
{

class RouteRequest;
class GeoDataDocument;
class GeoDataLineString;

class MARBLE_EXPORT AlternativeRoutesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum WritePolicy {
        Instant,
        Lazy
    };

    /** Constructor */
    explicit AlternativeRoutesModel( QObject *parent = 0 );

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
      * @param document The route to add
      * @param policy In lazy mode (default), a short amount of time is waited for
      *   other addRoute() calls before adding the route to the model. Otherwise, the
      *   model is changed immediately.
      */
    void addRoute( GeoDataDocument* document, WritePolicy policy = Lazy );

    /** Remove all alternative routes from the model */
    void clear();

    GeoDataDocument* currentRoute();

    /** Returns the waypoints contained in the route as a linestring */
    static const GeoDataLineString* waypoints( const GeoDataDocument* document );

public Q_SLOTS:
    void setCurrentRoute( int index );

Q_SIGNALS:
    void currentRouteChanged( GeoDataDocument* newRoute );
    void currentRouteChanged( int index );

private Q_SLOTS:
    void addRestrainedRoutes();

private:
    class Private;
    Private *const d;
};

} // namespace Marble

#endif
