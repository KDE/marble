//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESMODEL_H
#define MARBLE_ECLIPSESMODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QPixmap>

#include "GeoDataCoordinates.h"
#include "MarbleModel.h"

class EclSolar;

namespace Marble
{

class EclipsesItem;
class GeoPainter;
class GeoDataCoordinates;

class EclipsesModel : public QAbstractItemModel 
{
    Q_OBJECT
public:
    EclipsesModel( const MarbleModel *model, QObject *parent = 0 );
    ~EclipsesModel();

    const GeoDataCoordinates& observationPoint() const;
    void setObservationPoint( const GeoDataCoordinates &coords );

    void setYear( int year );
    int year() const;

    EclipsesItem* eclipseWithIndex( int year, int index );

    QList<EclipsesItem*> items() const;

    // QT model interface
    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex &index ) const;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &index,
                   int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const;

private:
    void paintItem( EclipsesItem *item, GeoPainter *painter );
    void addItem( EclipsesItem *item );
    void clear();
    void update();

    const MarbleModel *m_marbleModel;
    EclSolar *m_ecps;
    QList<EclipsesItem*> m_items;
    int m_currentYear;
    GeoDataCoordinates m_observationPoint;
};

}

#endif // MARBLE_ECLIPSESMODEL_H
