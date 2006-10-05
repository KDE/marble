#ifndef PLACECONTAINER_H
#define PLACECONTAINER_H

#include "placemark.h"
#include <QVector>

#include <QDebug>

class PlaceContainer : public QVector<PlaceMark*> {


public:
	PlaceContainer();
	PlaceContainer( QString name );

	void setName( QString name ){ m_name = name; }
	QString name() const {return m_name; }
	void clearTextPixmaps() { foreach ( PlaceMark* mark, *this ) mark->clearTextPixmap(); }

protected:	
	QString m_name;
};

#endif // PLACECONTAINER_H
