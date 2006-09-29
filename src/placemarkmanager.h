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

#include "katlasdirs.h"

/**
@author Torsten Rahn
*/

class PlaceMarkManager {
public:
	PlaceMarkManager();
	~PlaceMarkManager(){};

	void queryStatus();
};

#endif // PLACEMARKMANAGER_H
