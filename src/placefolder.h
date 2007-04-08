#ifndef PLACEFOLDER_H
#define PLACEFOLDER_H

#include <QtCore/QDebug>
#include <QtCore/QVector>

#include "placemark.h"


class PlaceFolder : public QVector<PlaceMark*> {


public:
	PlaceFolder();
	PlaceFolder( QString name );

	void setName( QString name ){ m_name = name; }
	QString name() const {return m_name; }

protected:	
	QString m_name;
};

#endif // PLACEFOLDER_H
