//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_PLACEMARKMANAGER_H
#define MARBLE_PLACEMARKMANAGER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

namespace Marble
{

class FileManager;
class GeoDataContainer;
class GeoDataDocument;
class GeoDataPlacemark;
class MarblePlacemarkModel;
class MarbleDataFacade;
class PlacemarkManagerPrivate;

/**
 * This class is responsible for loading the
 * place mark objects from the different files
 * and file formats.
 *
 * The loaded data are accessible via the
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
     * Sets the FileManager from which the placemarks are loaded.
     */
    void setFileManager ( FileManager *fileManager );

public Q_SLOTS:
    void addGeoDataDocument( int index );
    void removeGeoDataDocument( int index );

 private:
    QVector<GeoDataPlacemark*> recurseContainer(GeoDataContainer *container);
    void setupStyle( GeoDataDocument *doc, QVector<GeoDataPlacemark*> &container );
    void createFilterProperties( QVector<GeoDataPlacemark*> &container );
    int cityPopIdx( qint64 population ) const;
    int spacePopIdx( qint64 diameter ) const;
    int areaPopIdx( qreal area ) const;

    Q_DISABLE_COPY( PlacemarkManager )

    PlacemarkManagerPrivate *const d;
};

}

#endif
