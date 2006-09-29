//
// C++ Interface: gpplacecomposer
//
// Description: PlaceComposer 
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution

#ifndef PLACECOMPOSER_H
#define PLACECOMPOSER_H

#include "katlasdirs.h"

/**
@author Torsten Rahn
*/

class PlaceList;
class QStringListModel;

class PlaceComposer {
public:
	PlaceComposer();
	~PlaceComposer(){};

	void getPlaceMarks(QStringListModel*);
private:
	PlaceList* cities;
};

#endif // PLACECOMPOSER_H
