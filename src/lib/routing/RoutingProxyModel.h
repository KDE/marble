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

#ifndef MARBLE_ROUTING_DIRECTION_MODEL_H
#define MARBLE_ROUTING_DIRECTION_MODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace Marble {

/**
  * @brief A QAbstractItemModel that contains a list of routing instructions.
  *
  * Can be used in conjunction with RoutingModel to filter out waypoints
  * that don't contain instructions (a list of "important" parts of the route is left)
  *
  * @see RoutingModel
  */
class RoutingProxyModel : public QSortFilterProxyModel
{
public:
    /** Constructor */
    explicit RoutingProxyModel(QObject *parent = 0);

protected:
    /** Reimplemened from QSortFilterProxyModel. Filters waypoints without instructions */
    virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

};

} // namespace Marble

#endif // MARBLE_ROUTING_DIRECTION_MODEL_H
