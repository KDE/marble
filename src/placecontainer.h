#ifndef PLACECONTAINER_H
#define PLACECONTAINER_H

#include "placemark.h"
#include <QVector>

#include <QDebug>

inline bool popLessThan( PlaceMark* mark1, PlaceMark* mark2 ){ return mark1->population() > mark2->population(); }

class PlaceContainer : public QVector<PlaceMark*> {
public:
	PlaceContainer();
	PlaceContainer( QString name );

	void setName( QString name ){ m_name = name; }
	QString name() const {return m_name; }
	inline void clearTextPixmaps() { foreach ( PlaceMark* mark, *this ) mark->clearTextPixmap(); }
	inline void sort() { qStableSort( begin(), end(), popLessThan ); }
protected:	
	QString m_name;
};

#endif // PLACECONTAINER_H
