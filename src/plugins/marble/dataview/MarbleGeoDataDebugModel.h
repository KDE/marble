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

#ifndef MarbleGeoDataDebugModel_H
#define MarbleGeoDataDebugModel_H


#include <QtCore/QAbstractItemModel>

namespace Marble
{
class GeoDataDocument;

/**
 * @short The representation of GeoData in a model
 * This class represents all available data given by kml-data files.
 */
class MarbleGeoDataDebugModel : public QAbstractItemModel
{
    Q_OBJECT

 public:

    /**
     * Creates a new GeoDataModel.
     *
     * @param parent The parent object.
     */
    explicit MarbleGeoDataDebugModel( GeoDataDocument*, QObject *parent = 0 );

    /**
     * Destroys the GeoDataModel.
     */
    ~MarbleGeoDataDebugModel();

    /**
     * Return the number of Items in the Model.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    /**
    * Return the 
    */
    QVariant data( const QModelIndex &index, int role ) const;

    QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;

    QModelIndex parent(const QModelIndex &index) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void reset() { QAbstractItemModel::reset(); }

    /**
    * Return a pointer to the root object of a geodata tree.
    * This tree is a merged version of all the files loaded into this model
    */
    Marble::GeoDataDocument* geoDataRoot();
    
  public Q_SLOTS:
    void update();
    
 private:
    Q_DISABLE_COPY( MarbleGeoDataDebugModel )
    class Private;
    Private* const d;
};

}

#endif // MarbleGeoDataDebugModel_H
