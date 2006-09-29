#ifndef PLACEMARKSTORAGE_H
#define PLACEMARKSTORAGE_H

#include <QObject>

#include <QDebug>

class PlaceMarkStorage : QObject {

public:
	PlaceMarkStorage();
	PlaceMarkStorage( QString );

	void setName( QString name ){ m_name = name; }
	QString name() const {return m_name; }
private:	
	QString m_name;
};

#endif // PLACEMARKSTORAGE_H
