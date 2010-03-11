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

#ifndef MARBLE_ROUTING_MODEL_H
#define MARBLE_ROUTING_MODEL_H

#include "GeoDataLineString.h"
#include "MarblePlacemarkModel.h"

#include <QtCore/QAbstractListModel>

/**
  * A QAbstractItemModel that contains a list of routing instructions.
  * Each item represents a routing step in the way from source to
  * destination. Steps near the source come first, steps near the target
  * last.
  */
namespace Marble {

class RoutingModelPrivate;
class GeoDataDocument;

class RoutingModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoutingItemDataRole {
        CoordinateRole = MarblePlacemarkModel::CoordinateRole, // synchronized with MarblePlacemarkModel
        TypeRole = CoordinateRole + 24 // avoid conflict with MarblePlacemarkModel
    };

    enum RoutingItemType {
        WayPoint,
        Instruction,
        Start,
        Via,
        Destination
    };

    /** Constructor */
    explicit RoutingModel(QObject *parent = 0);

    /** Destructor */
    ~RoutingModel();

    // Model querying

    /** Overload of QAbstractListModel */
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    /** Overload of QAbstractListModel */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /** Overload of QAbstractListModel */
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    // Model editing

    /** Overload of QAbstractListModel */
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    /** Overload of QAbstractListModel */
    Qt::ItemFlags flags ( const QModelIndex & index ) const;

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
    void importOpenGis( const QByteArray &xmlData, const GeoDataLineString &route);

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

private:
    RoutingModelPrivate* const d;
};

} // namespace Marble

Q_DECLARE_METATYPE( Marble::RoutingModel::RoutingItemType )

#endif // MARBLE_ROUTING_MODEL_H
