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

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtCore/QVector>

class BoundingBox;
class GpxFile;

/**
*	@author Andrew Manson <g.real.ate@gmail.com>
*/
class GpxFileModel : public QAbstractItemModel
{
     Q_OBJECT

Q_SIGNALS :
    void updateRegion( BoundingBox& );
    void enableActions( bool );
public Q_SLOTS:
    void saveFile();
    void closeFile();
public:
    explicit GpxFileModel( QObject *parent = 0 );

    ~GpxFileModel();
    
    Qt::ItemFlags flags( const QModelIndex &item) const ;
    
    QVariant data( const QModelIndex &index, 
                   int role = Qt::DisplayRole ) const;
    bool setData ( const QModelIndex & index, const QVariant & value,
                   int role = Qt::EditRole );
    QModelIndex index ( int row, int column, 
                        const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & index ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() )     const;
    int columnCount ( const QModelIndex & parent = QModelIndex() )  const;
    void addFile( GpxFile *file);
    void setSelectedIndex( const QModelIndex &);
    QVector<GpxFile*>* allFiles();

 private:
    QModelIndex         m_selectedIndex;
    QVector<GpxFile*>  *m_data;
};


#endif
