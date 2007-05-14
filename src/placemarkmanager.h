//
// C++ Interface: gpplacemarkmanager
//
// Description: PlaceMarkManager
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution


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
