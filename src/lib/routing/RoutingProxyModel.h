//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTINGPROXYMODEL_H
#define MARBLE_ROUTINGPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace Marble
{

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
    explicit RoutingProxyModel( QObject *parent = 0 );

protected:
    /** Reimplemened from QSortFilterProxyModel. Filters waypoints without instructions */
    virtual bool filterAcceptsRow ( int source_row, const QModelIndex &source_parent ) const;

};

} // namespace Marble

#endif
