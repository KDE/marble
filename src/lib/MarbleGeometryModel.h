//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin  <ps_ml@gmx.de>
// Copyright 2008      Simon Schmeisser <mail_to_wrt@gmx.de>
//

#ifndef MarbleGeometryModel_H
#define MarbleGeometryModel_H

#include <QtCore/QAbstractItemModel>

namespace Marble
{
class GeoDataDocument;

/**
 * @short The representation of GeoData in a model
 * This class represents all available data given by kml-data files.
 */
class MarbleGeometryModel : public QAbstractItemModel
{
    Q_OBJECT

 public:

    /**
     * Creates a new MarbleGeometryModel.
     *
     * @param parent The parent object.
     */
    explicit MarbleGeometryModel( GeoDataDocument* root, QObject *parent = 0 );
    
    /**
     * Creates a new MarbleGeometryModel.
     *
     * @param parent The parent object.
     */
    explicit MarbleGeometryModel( QObject *parent = 0 );

    /**
     * Destroys the MarbleGeometryModel.
     */
    ~MarbleGeometryModel();
    
    /**
     * set the geodataroot tree and initialize the model
     */
    void setGeoDataRoot( GeoDataDocument* root );
    
    /**
    * return the value of the root document currently used
    */
    GeoDataDocument* geoDataRoot() const;
 
    /**
    * Return the data of an @p index with the specified @p role
    *
    * @param index the index of the wished data
    * @param role one role which controls the return value
    */
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    /**
     * Return the index of the child in @p row and @p column of the item @p parent
     * @param parent a QModelIndex pointing to the parent
     */
    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const;

    /**
     * Return the parent index of another index
     */
    QModelIndex parent( const QModelIndex &index ) const;

    /**
     * the number of columns in this model (currently set to hardcoded 1)
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;

    /**
     * Return the number of items in the model.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    void reset() { QAbstractItemModel::reset(); }

  public Q_SLOTS:
    void update();
    
 private:
    Q_DISABLE_COPY( MarbleGeometryModel )
    class Private;
    Private* const d;
};

}

#endif // MarbleGeometryModel_H
