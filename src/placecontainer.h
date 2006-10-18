#ifndef PLACECONTAINER_H
#define PLACECONTAINER_H

#include "placemark.h"
#include <QVector>

#include <QDebug>

class PlaceContainer : public QVector<PlaceMark*> {

protected:	
	QString m_name;

public:
	PlaceContainer();
	PlaceContainer( QString name );

	void setName( QString name ){ m_name = name; }
	QString name() const {return m_name; }
	inline void clearTextPixmaps() { foreach ( PlaceMark* mark, *this ) mark->clearTextPixmap(); }
//	inline bool popLessThan( PlaceMark* mark1, PlaceMark* mark2 ){
//		return mark1->population() < mark2->population(); 
//		return true;
//	}
//	QVectorIterator<PlaceMark*> i;
//	inline void sort() { qStableSort( begin(), end(), popLessThan ); }
};

#endif // PLACECONTAINER_H
