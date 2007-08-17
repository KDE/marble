//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
//

#include "GpxFileModel.h"
#include <QtCore/Qt>
#include <QDebug>

GpxFileModel::GpxFileModel()
 : QAbstractItemModel()
{
    m_data = new QVector<GpxFile*>();
}


GpxFileModel::~GpxFileModel()
{
}

Qt::ItemFlags GpxFileModel::flags( const QModelIndex &item ) const
{
    return (static_cast<GpxFile*>(item.internalPointer()) ) ->flags()
;
}

QVariant GpxFileModel::data( const QModelIndex &index, int role ) 
                                                                const
{
    if ( !index.isValid() ) {
        return QVariant();
    }
    
    if ( role == Qt::DisplayRole ) {
        
        return static_cast<GpxFile*> ( index.internalPointer() )
                                                          ->display();
    }
    
    if ( role == Qt::CheckStateRole ) {
        return static_cast<GpxFile*>( index.internalPointer() )
                                                     ->checkState();
    }
    
    return QVariant();
}

bool GpxFileModel::setData ( const QModelIndex &index, 
                             const QVariant &value,
                             int role = Qt::EditRole )
{
    if ( role == Qt::CheckStateRole ) {
        ( static_cast<GpxFile*>( index.internalPointer() ) ) 
                ->setCheckState( value.toBool() );
        emit ( dataChanged( index, index ) );
        emit ( updateRegion( BoundingBox() ) );
        return true;
    }
    
    return false;
}

QModelIndex GpxFileModel::index ( int row, int column, 
                                  const QModelIndex &parent ) const 
{
    if ( !hasIndex( row, column, parent ) ) {
        return QModelIndex();
    }
    
    return createIndex ( row, column, m_data->at( row ) );
}

QModelIndex GpxFileModel::parent ( const QModelIndex & index ) const
{
    return QModelIndex();
}

int GpxFileModel::rowCount ( const QModelIndex & parent ) const
{
    if ( !parent.isValid() ) {
        return m_data->count();
    }
    
    return 0;
}

int GpxFileModel::columnCount ( const QModelIndex & parent ) const 
{
    return 1;
}

void GpxFileModel::addFile( GpxFile *file )
{
    m_data->append( file );
    emit( layoutChanged() );
}

QVector<GpxFile*> * GpxFileModel::allFiles()
{
    return m_data;
}

  #include "GpxFileModel.moc"


