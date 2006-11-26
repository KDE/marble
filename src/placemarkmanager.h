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

class PlaceMarkManager {
public:
	PlaceMarkManager();
	~PlaceMarkManager(){};

	void addPlaceMarkFile( QString );

	PlaceContainer* getPlaceContainer(){ m_placecontainer->sort(); return m_placecontainer; }

	void loadKml( QString );

protected:
	PlaceContainer* m_placecontainer;

	void importKml( QString, PlaceContainer* );
	void saveFile( QString, PlaceContainer* );
	bool loadFile( QString, PlaceContainer* );
};

#endif // PLACEMARKMANAGER_H
