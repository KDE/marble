//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESCONFIGMODEL_H
#define MARBLE_SATELLITESCONFIGMODEL_H

#include <QtCore/QModelIndex>

#include "SatellitesConfigNodeItem.h"

namespace Marble {

class SatellitesConfigModel : public QAbstractItemModel
{

public:
    explicit SatellitesConfigModel( QObject *parent = 0 );
    ~SatellitesConfigModel();

    void loadSettings( QHash<QString, QVariant> settings );

    void appendChild( SatellitesConfigAbstractItem *child );
    void clear();

    QStringList idList() const;
    QStringList fullIdList() const;
    QStringList urlList() const;

    QVariant data( const QModelIndex &index,
                   int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex &index,
                  const QVariant &value,
                  int role = Qt::EditRole );
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex &child ) const;
    QModelIndex index( int row,
                       int column,
                       const QModelIndex &parent = QModelIndex() ) const;
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags( const QModelIndex &index ) const;
    SatellitesConfigNodeItem* rootItem() const;

protected:
    SatellitesConfigNodeItem *m_rootItem;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGMODEL_H

