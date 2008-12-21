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

#include "PlaceMarkLoader.h"

namespace Marble
{

class GeoDataDocument;
class PlaceMarkContainer;
class MarblePlacemarkModel;
class MarbleGeometryModel;

/**
 * This class is responsible for loading the
 * place mark objects from the different files
 * and file formats.
 *
 * The loaded data are accessable via the
 * MarblePlacemarkModel returned by model().
 */
class PlaceMarkManager : public QObject
{
    friend class MarblePlacemarkModel;
    Q_OBJECT

 public:
    /**
     * Creates a new place mark manager.
     *
     * @param parent The parent object.
     */
    explicit PlaceMarkManager( QObject *parent = 0 );

    /**
     * Destroys the place mark manager.
     */
    ~PlaceMarkManager();

    /**
     * Returns the model which represents the data of the
     * place mark manager.
     *
     * Note: The manager has not the ownership of the model.
     */
    MarblePlacemarkModel *model() const;

    /**
     * Returns the GeometryModel which represents the GeoData tree of the
     * place mark manager. The above MarblePlacemarkModel only represents
     * a part of the data (namely Placemarks)
     *
     * Note: The manager has not the ownership of the model.
     */
    MarbleGeometryModel *geomodel() const;
    
    /**
     * Sets the GeometryModel to which the graphical views can be attached.
     */
    void setGeoModel( MarbleGeometryModel * model );

    void clearPlaceMarks();

    /**
     * This methods loads the standard place mark files.
     *
     * Note: Call this method after you have create a method!
     */
    void loadStandardPlaceMarks( const QString& target = QString() );

    /**
     * Loads a new place mark file into the manager.
     */
    void addPlaceMarkFile( const QString &fileName );

    /**
     * Loads a new KML file into the manager.
     */
    void loadKml( const QString &fileName, bool clearPrevious = false );

    /**
     * Loads a new KML data as string into the manager.
     */
    void loadKmlFromData( const QString& data, bool clearPrevious = false );

 Q_SIGNALS:
    void geoDataDocumentLoaded( GeoDataDocument& );

 private Q_SLOTS:
    void loadPlaceMarkContainer( PlaceMarkLoader* loader, PlaceMarkContainer * );

 private:
    void importKml( const QString&, PlaceMarkContainer* );
    void importKmlFromData( const QString&, PlaceMarkContainer* );
    void saveFile( const QString&, PlaceMarkContainer* );
    bool loadFile( const QString&, PlaceMarkContainer* );

    void setPlaceMarkModel( MarblePlacemarkModel *model );

    Q_DISABLE_COPY( PlaceMarkManager )
    MarblePlacemarkModel* m_model;
    MarbleGeometryModel* m_geomodel;
    QList<PlaceMarkLoader*> m_loaderList;

    QString m_target;
};

}

#endif // PLACEMARKMANAGER_H
