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

#include <QModelIndex>

#include "SatellitesConfigNodeItem.h"

namespace Marble {

class SatellitesConfigModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit SatellitesConfigModel( QObject *parent = 0 );
    ~SatellitesConfigModel() override;

    void loadSettings(const QHash<QString, QVariant> &settings);

    void appendChild( SatellitesConfigAbstractItem *child );
    void clear();

    QStringList idList() const;
    QStringList fullIdList() const;
    QStringList urlList() const;

    QVariant data( const QModelIndex &index,
                   int role = Qt::DisplayRole ) const override;
    bool setData( const QModelIndex &index,
                  const QVariant &value,
                  int role = Qt::EditRole ) override;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QModelIndex parent( const QModelIndex &child ) const override;
    QModelIndex index( int row,
                       int column,
                       const QModelIndex &parent = QModelIndex() ) const override;
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const override;
    Qt::ItemFlags flags( const QModelIndex &index ) const override;
    SatellitesConfigNodeItem* rootItem() const;

protected:
    SatellitesConfigNodeItem *m_rootItem;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGMODEL_H

