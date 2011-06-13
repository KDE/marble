//
// This file is part of the Marble Virtual Globe.
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

/**
 * @short The representation of GeoData in a model
 * This class represents all available data given by kml-data files.
 */
class GeoDataTreeModel : public QAbstractItemModel
{
    Q_OBJECT

 public:

    /**
     * Creates a new GeoDataTreeModel.
     *
     * @param parent The parent object.
     */
    explicit GeoDataTreeModel( QObject *parent = 0 );

    /**
     * Destroys the GeoDataModel.
     */
    ~GeoDataTreeModel();

    virtual bool hasChildren( const QModelIndex &parent ) const;

    /**
     * Return the number of Items in the Model.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    QVariant data( const QModelIndex &index, int role ) const;

    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const;

    QModelIndex parent( const QModelIndex &index ) const;

    int columnCount( const QModelIndex &parent = QModelIndex() ) const;

    Qt::ItemFlags flags ( const QModelIndex & index ) const;

    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    void reset() { QAbstractItemModel::reset(); }

    /**
      * Sets the root document to use. This replaces previously loaded data, if any.
      * @param document The new root document. Ownership retains with the caller,
      *   i.e. GeoDataTreeModel will not delete the passed document at its destruction.
      */
    void setRootDocument( GeoDataDocument* document );

    int addDocument( GeoDataDocument* document );

    void removeDocument( int index );

    void removeDocument( GeoDataDocument* document );

public Q_SLOTS:
    void update();
    
signals:
    /// insert and remove row don't trigger any signal that proxies forward
    /// this signal will refresh geometry layer and placemark layout
    void treeChanged();

 private:
    Q_DISABLE_COPY( GeoDataTreeModel )
    class Private;
    Private* const d;
};

}

#endif // MARBLE_GEODATATREEMODEL_H
