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

GpxFileModel::GpxFileModel()
 : QAbstractItemModel()
{
    m_data = new QVector<GpxFile*>();
}


GpxFileModel::~GpxFileModel()
{
}

Qt::ItemFlags flags( const QModelIndex &item )
{
    return ( ( GpxFile *)item.internalPointer() )->flags() ;
}

QVariant GpxFileModel::data( const QModelIndex &index, int role ) 
                                                                const
{/*
    switch ( role ) {
    case Qt::DisplayRole:
        return ( ( GpxFile *)index.internalPointer() ) -> display();
    }*/
    return QString("test");
    
}

QModelIndex GpxFileModel::index ( int row, int column, 
                                  const QModelIndex &parent ) const 
{
    QModelIndex temp;
    
    if ( row <= m_data->size() ) {
        temp = createIndex ( row, column, m_data->at( row ) );
    }
    
    return temp;
}
QModelIndex GpxFileModel::parent ( const QModelIndex & index ) const
{
    return QModelIndex();
}

int GpxFileModel::rowCount ( const QModelIndex & parent ) const
{
    return m_data->size();
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


