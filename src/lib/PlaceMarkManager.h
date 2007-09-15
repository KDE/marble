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


#include <QtCore/QMap>
#include "PlaceMarkContainer.h"
#include "MarbleDirs.h"
#include "kml/KMLDocument.h"

class KMLDocument;

class PlaceMarkManager: public QObject
{
    Q_OBJECT

 public:
    PlaceMarkManager( QObject *parent = 0 );
    ~PlaceMarkManager(){}

    void addPlaceMarkFile( const QString& );

    PlaceMarkContainer* getPlaceMarkContainer() {
        m_placeMarkContainer->sort();
        return m_placeMarkContainer;
    }

    void loadKml( const QString& );

#ifdef KML_GSOC
    const QList < KMLFolder* >& getFolderList() const;
#endif

 protected:
    void importKml( const QString&, PlaceMarkContainer* );
    void saveFile( const QString&, PlaceMarkContainer* );
    bool loadFile( const QString&, PlaceMarkContainer* );

#ifdef KML_GSOC
 private:
    void updateCacheIndex();
    void cacheDocument( const KMLDocument& document );
    void loadDocumentFromCache ( QString &path, KMLDocument& document );
#endif

 protected:
    // Eventually there will be more than one container.
    PlaceMarkContainer  *m_placeMarkContainer;

#ifdef KML_GSOC
 private:
    QList < KMLDocument* > m_documentList;
#endif

};

#endif // PLACEMARKMANAGER_H
