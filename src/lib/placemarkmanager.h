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


#include "PlaceMarkContainer.h"
#include "katlasdirs.h"


class PlaceMarkManager
{
 public:
    PlaceMarkManager();
    ~PlaceMarkManager(){}

    void addPlaceMarkFile( const QString& );

    PlaceMarkContainer* getPlaceMarkContainer() {
        m_placeMarkContainer->sort();
        return m_placeMarkContainer;
    }

    void loadKml( const QString& );

 protected:
    void importKml( const QString&, PlaceMarkContainer* );
    void saveFile( const QString&, PlaceMarkContainer* );
    bool loadFile( const QString&, PlaceMarkContainer* );

 protected:
    // Eventually there will be more than one container.
    PlaceMarkContainer  *m_placeMarkContainer;
};

#endif // PLACEMARKMANAGER_H
