//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Thibaut Gridel <tgridel@free.fr>
//

#ifndef MARBLE_GEODATATREEMODEL_H
#define MARBLE_GEODATATREEMODEL_H


#include <QtCore/QAbstractItemModel>

namespace Marble
{
class GeoDataDocument;
class FileManager;

/**
 * @short The representation of GeoData in a model
 * This class represents all available data given by kml-data files.
 */
class GeoDataTreeModel : public QAbstractItemModel
{
    Q_OBJECT

 public:

    /**
     * Creates a new GeoDataModel.
     *
     * @param parent The parent object.
     */
    explicit GeoDataTreeModel( QObject *parent = 0 );

    /**
     * Destroys the GeoDataModel.
     */
    ~GeoDataTreeModel();

    /**
     * Return the number of Items in the Model.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    QVariant data( const QModelIndex &index, int role ) const;

    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const;

    QModelIndex parent( const QModelIndex &index ) const;

    int columnCount( const QModelIndex &parent = QModelIndex() ) const;

    void reset() { QAbstractItemModel::reset(); }

    /**
     * Sets the FileManager from which the documents are loaded.
     */
    void setFileManager ( FileManager *fileManager );


  public Q_SLOTS:
    void addDocument( int index );

    void removeDocument( int index );

    void update();
    
 private:
    Q_DISABLE_COPY( GeoDataTreeModel )
    class Private;
    Private* const d;
};

}

#endif // MARBLE_GEODATATREEMODEL_H
