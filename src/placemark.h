#ifndef PLACEMARK_H
#define PLACEMARK_H

// #include "geopoint.h"
// #include <QDebug>
#include <QChar>
#include <QPixmap>
#include <QRect>

#include <QVector>

#include "geopoint.h"
#include "placemarkstorage.h"

class PlaceMark : public PlaceMarkStorage {


public:
	PlaceMark();
	PlaceMark( QString );

	void coordinate( float& lng, float& lat );
	void setCoordinate( float lng, float lat );

	const QPoint& symbolPos() const { return m_sympos; }
	void setSymbolPos( const QPoint& sympos ){ m_sympos = sympos; }

	const QChar role() const { return m_role; }
	void setRole( QChar role ){ m_role = role; }

	const int symbol() const { return m_symbol; }
	void setSymbol( int symbol ){ m_symbol = symbol; }

	const int popidx() const { return m_popidx; }
	void setPopidx( int popidx ){ m_popidx = popidx; }

	const int population() const { return m_population; }
	void setPopulation( int population ){ m_population = population; }

	const bool selected() const { return m_selected; }
	void setSelected( bool selected ){ m_selected = selected; }

	const Quaternion& getQuatPoint() const { return m_coordinate.getQuatPoint(); }	

	const QPixmap& textPixmap() const { return m_textPixmap; }
	const QRect& textRect() const { return m_rect; }
	void setTextPixmap( QPixmap& textPixmap ){ m_textPixmap = textPixmap; }
	void setTextRect( const QRect& textRect ){ m_rect = textRect;}
	void clearTextPixmap(){ if ( m_textPixmap.isNull() == false ) { m_textPixmap = QPixmap(); } }

private:
	GeoPoint m_coordinate;
	QPoint m_sympos;
	QPixmap m_textPixmap;
	QRect m_rect;
	int m_population;
	bool m_selected;
	int m_symbol;
	int m_popidx;
	QChar m_role;

};

#endif // PLACEMARK_H
