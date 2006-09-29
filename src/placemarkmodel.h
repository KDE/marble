//
// C++ Interface: gpmapscale
//
// Description: PlaceMarkModel 

// PlaceMarkMode resembles the model for Placemarks
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef PLACEMARKMODEL_H
#define PLACEMARKMODEL_H

#include <QStandardItemModel>
/**
@author Torsten Rahn
*/

class PlaceMarkModel : public QStandardItemModel {

Q_OBJECT

public:
	PlaceMarkModel(QObject *parent = 0);

	void load(QString);
private:

};

#endif // PLACEMARKMODEL_H
