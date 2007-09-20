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

#include <QtCore/QObject>

class KMLDocument;
class PlaceMarkContainer;
class PlaceMarkModel;

/**
 * This class is responsible for loading the
 * place mark objects from the different files
 * and file formats.
 *
 * The loaded data are accessable via the
 * PlaceMarkModel returned by model().
 */
class PlaceMarkManager : public QObject
{
    friend class PlaceMarkModel;
    Q_OBJECT

 public:
    /**
     * Creates a new place mark manager.
     *
     * @param parent The parent object.
     */
    PlaceMarkManager( QObject *parent = 0 );

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
    PlaceMarkModel *model() const;

    /**
     * This methods loads the standard place mark files.
     *
     * Note: Call this method after you have create a method!
     */
    void loadStandardPlaceMarks();

    /**
     * Loads a new place mark file into the manager.
     */
    void addPlaceMarkFile( const QString &fileName );

#ifdef KML_GSOC
    const QList < KMLFolder* >& getFolderList() const;
#endif

    /**
     * Loads a new KML file into the manager.
     */
    void loadKml( const QString &fileName );

 Q_SIGNALS:
    void kmlDocumentLoaded( KMLDocument& );

 private:
    void importKml( const QString&, PlaceMarkContainer* );
    void saveFile( const QString&, PlaceMarkContainer* );
    bool loadFile( const QString&, PlaceMarkContainer* );

    void setPlaceMarkModel( PlaceMarkModel *model );

    PlaceMarkModel* m_model;

#ifdef KML_GSOC
    void updateCacheIndex();
    void cacheDocument( const KMLDocument& document );
    void loadDocumentFromCache ( QString &path, KMLDocument& document );

    QList < KMLDocument* > m_documentList;
#endif
};

#endif // PLACEMARKMANAGER_H
