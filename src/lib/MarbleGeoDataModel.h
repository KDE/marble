//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin  <ps_ml@gmx.de>
//

#ifndef MarbleGeoDataModel_H
#define MarbleGeoDataModel_H


#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>

#include "marble_export.h"

class QString;

namespace Marble
{
class GeoDataDocument;

/**
 * @short The representation of GeoData in a model
 * This class represents all available data given by kml-data files.
 * It will encapsulate the different Tiles needed to form one layer.
 * Thus each layer will be represented by one MarbleGeoDataModel.
 */
class MARBLE_EXPORT MarbleGeoDataModel : public QAbstractListModel
{
    Q_OBJECT
 public:

    /**
     * Creates a new GeoDataModel.
     *
     * @param parent The parent object.
     */
    explicit MarbleGeoDataModel( QObject *parent = 0 );

    /**
     * Destroys the GeoDataModel.
     */
    ~MarbleGeoDataModel();
	
    /**
     * Return the number of Items in the Model.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    
    /**
    * Return the 
    */
    QVariant data( const QModelIndex &index, int role ) const;

    /**
    * @brief Load another kml file and merge it into the current geoDataRoot
    * The styles, styleMaps and features are merged into the geoDataRoot document
    * the return value is a unique id given for each loaded document 
    * @param filename the filename in the local filesystem
    */
    unsigned long addGeoDataFile( const QString &filename );

    /**
    * @brief Unload a document from geoDataRoot
    * a specific kml file is unmerged from the current geoDataRoot
    * please keep in mind that styles and styleMaps added by this document
    * will become invalid when the document is removed
    * @param keyId the Id given when adding the GeoDataDocument
    */
    bool removeGeoDataFile( unsigned long keyId );

    /**
    * Return a pointer to the root object of a geodata tree.
    * This tree is a merged version of all the files loaded into this model
    */
    Marble::GeoDataDocument* geoDataRoot();
 Q_SIGNALS:
 
    /**
     * Emitted whenever the data in the Model has changed
     */
    void dataChanged();
 
 private:
    Q_DISABLE_COPY( MarbleGeoDataModel )
    class Private;
    Private* const d;
};

}

#endif // MarbleGeoDataModel_H
