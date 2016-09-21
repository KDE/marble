//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Mikhail Ivchenko  <ematirov@gmail.com>
//

#ifndef NODEMODEL_H
#define NODEMODEL_H

#include "GeoDataCoordinates.h"
#include <QAbstractListModel>

namespace Marble
{

class NodeModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit NodeModel( QObject *parent = 0 );
    int rowCount( const QModelIndex &parent ) const;
    int columnCount( const QModelIndex &parent ) const;
    QVariant data( const QModelIndex &index, int role ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    void clear();
    Qt::ItemFlags flags(const QModelIndex & index) const ;
public Q_SLOTS:
    int addNode( const GeoDataCoordinates &node );
private:
    QVector<GeoDataCoordinates> m_nodes;
};

}

#endif
