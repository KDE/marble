//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
//

#ifndef GPXFILEMODEL_H
#define GPXFILEMODEL_H

#include "GpxFile.h"

#include <QAbstractItemModel>
#include <QtCore/QVariant>
#include <QtCore/QVector>

/**
*	@author Andrew Manson <g.real.ate@gmail.com>
*/
class GpxFileModel : public QAbstractItemModel
{
public:
    GpxFileModel();

    ~GpxFileModel();
    
    Qt::ItemFlags flags( const QModelIndex &item);
    
    QVariant data( const QModelIndex &index, 
                   int role = Qt::DisplayRole ) const;
    QModelIndex index ( int row, int column, 
                        const QModelIndex &parent = QModelIndex() )
                                                                const;
    QModelIndex parent ( const QModelIndex & index ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() )
                                                                const;
    void addFile( GpxFile *file);
    QVector<GpxFile*>* allFiles();
    private:
    QVector<GpxFile*>    *m_data;

};

#endif
