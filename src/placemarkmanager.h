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


#include "placecontainer.h"
#include "katlasdirs.h"


/**
@author Torsten Rahn
*/

class PlaceMarkManager
{
 public:
    PlaceMarkManager();
    ~PlaceMarkManager(){};

    void addPlaceMarkFile( const QString& );

    PlaceContainer* getPlaceContainer() {
        m_placecontainer->sort();
        return m_placecontainer;
    }

    void loadKml( const QString& );

 protected:
    void importKml( const QString&, PlaceContainer* );
    void saveFile( const QString&, PlaceContainer* );
    bool loadFile( const QString&, PlaceContainer* );

 protected:
    PlaceContainer  *m_placecontainer;
};

#endif // PLACEMARKMANAGER_H
