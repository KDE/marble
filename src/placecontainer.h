#ifndef PLACECONTAINER_H
#define PLACECONTAINER_H

#include "placemark.h"
#include <QVector>

#include <QDebug>

inline bool popLessThan( PlaceMark* mark1, PlaceMark* mark2 ){ 

	return  mark1->selected() == mark2->selected() ? mark1->population() > mark2->population() :
	mark1->selected() == 1 ? true : false;

	/* 
	 *  If compared items don't differ in terms of being selected
	 *  compare them based on population numbers.
	 *  If compared items do differ in terms of being selected
	 *  then let that be the final criteria.
	 */
}

class PlaceContainer : public QVector<PlaceMark*> {
public:
	PlaceContainer();
	PlaceContainer( QString name );

	void setName( QString name ){ m_name = name; }
	QString name() const {return m_name; }
	inline void clearTextPixmaps() { foreach ( PlaceMark* mark, *this ) mark->clearTextPixmap(); }

	inline void deleteAll() { foreach ( PlaceMark* mark, *this ){ if ( mark != 0 ) delete mark; } }

	inline void clearSelected() { foreach ( PlaceMark* mark, *this ){ if ( mark != 0 ) mark->setSelected( 0 ); } }

	inline void sort() { qStableSort( begin(), end(), popLessThan ); }
protected:	
	QString m_name;
};

#endif // PLACECONTAINER_H
