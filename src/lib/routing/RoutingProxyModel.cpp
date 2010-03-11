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

#include "RoutingProxyModel.h"

#include "RoutingModel.h"

#include <QtCore/QDebug>

namespace Marble {

RoutingProxyModel::RoutingProxyModel(QObject *parent) :
        QSortFilterProxyModel(parent)
{
    // nothing to do
}

bool RoutingProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
    Q_UNUSED(source_parent)
    if (!sourceModel())
        return false;

    QModelIndex index = sourceModel()->index(source_row, 0);
    RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>(index.data(RoutingModel::TypeRole));
    return type == RoutingModel::Instruction ||
           type == RoutingModel::Start ||
           type == RoutingModel::Destination;
}

} // namespace Marble
