#ifndef PLACEMARKSTORAGE_H
#define PLACEMARKSTORAGE_H

#include <QObject>

#include <QDebug>

class PlaceMarkStorage {

public:
	PlaceMarkStorage();
	PlaceMarkStorage( QString );

	void setName( QString name ){ m_name = name; }
	QString name() const {return m_name; }

	void setVisible( bool visible ){ m_visible = visible; }
	bool visible() const {return m_visible; }

private:	
	QString m_name;
	bool m_visible;
};

#endif // PLACEMARKSTORAGE_H
