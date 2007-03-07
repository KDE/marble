#ifndef PLACELIST_H
#define PLACELIST_H

#include "placemark.h"
#include <QtCore/QDebug>

class PlaceList : public QVector<PlaceMark*> {
private:	
	void load(QString);
public:
	PlaceList();
	QStringList getPlaceMarkList();
//	~PlaceList(){};
};

#endif // PLACELIST_H
