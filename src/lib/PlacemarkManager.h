//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef PLACEMARKMANAGER_H
#define PLACEMARKMANAGER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "PlacemarkLoader.h"

namespace Marble
{

class GeoDataDocument;
class PlacemarkContainer;
class MarblePlacemarkModel;
class MarbleDataFacade;
class PlacemarkManagerPrivate;

/**
 * This class is responsible for loading the
 * place mark objects from the different files
 * and file formats.
 *
 * The loaded data are accessable via the
 * MarblePlacemarkModel returned by model().
 */
class PlacemarkManager : public QObject
{
    friend class MarblePlacemarkModel;
    Q_OBJECT

 public:
    /**
     * Creates a new place mark manager.
     *
     * @param parent The parent object.
     */
    explicit PlacemarkManager( QObject *parent = 0 );

    /**
     * Destroys the place mark manager.
     */
    ~PlacemarkManager();

    /**
     * Returns the model which represents the data of the
     * place mark manager.
     *
     * Note: The manager has not the ownership of the model.
     */
    MarblePlacemarkModel *model() const;

    /**
     * Sets the DataFacade from which the models can be accessed.
     */
    void setDataFacade ( MarbleDataFacade *facade );

    /**
     * return a list containing all containers that have been queued for loading or that are
     * loaded already
     */
    QStringList containers() const;

    /**
     * Loads a new place mark file into the manager.
     */
    void addPlacemarkFile( const QString &fileName );

    /**
    * removes an existing GeoDataDocument from the manager
    */
    void removePlacemarkKey( const QString& key );

    /**
    * add Data containing KML code as string
    */
    void addPlacemarkData( const QString& data, const QString& key );

 Q_SIGNALS:
    void geoDataDocumentAdded( const GeoDataDocument& );
    void finalize();

 private Q_SLOTS:
    void loadPlacemarkContainer( PlacemarkLoader* loader, PlacemarkContainer * );
    void cleanupLoader( PlacemarkLoader* loader );
    void addGeoDataDocument( GeoDataDocument* );

 private:

    void appendLoader( PlacemarkLoader *loader );

    /**
     * internal helper function which returns the regular name of a kml or cache file
     */
    static QString toRegularName( QString name );

    Q_DISABLE_COPY( PlacemarkManager )

    PlacemarkManagerPrivate *const d;
};

}

#endif // PLACEMARKMANAGER_H
