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

	GeoPoint coordinate() const { return m_coordinate; } 
	void coordinate( float& lng, float& lat );
	void setCoordinate( float lng, float lat );

	const QPoint& symbolPos() const { return m_sympos; }
	void setSymbolPos( const QPoint& sympos ){ m_sympos = sympos; }

	const QChar role() const { return m_role; }
	void setRole( QChar role ){ m_role = role; }

	const QString description() const { return m_description; }
	void setDescription( QString description ){ m_description = description; }

	const QString countryCode() const { return m_countrycode; }
	void setCountryCode( QString countrycode ){ m_countrycode = countrycode; }

	const int symbol() const { return m_symbol; }
	void setSymbol( int symbol ){ m_symbol = symbol; }

	const QPixmap symbolPixmap() const;
	const QSize symbolSize() const;
//	void setSymbol( int symbol ){ m_symbol = symbol; }

	const int popidx() const { return m_popidx; }
	void setPopidx( int popidx ){ m_popidx = popidx; }

	const int population() const { return m_population; }
	void setPopulation( int population ){ m_population = population; }

	const int selected() const { return m_selected; }
	void setSelected( int selected ){ m_selected = selected; }

	const Quaternion& getQuatPoint() const { return m_coordinate.getQuatPoint(); }	

	const QPixmap& textPixmap() const { return m_labelPixmap; }
	const QRect& textRect() const { return m_rect; }
	void setTextPixmap( QPixmap& labelPixmap ){ m_labelPixmap = labelPixmap; }
	void setTextRect( const QRect& textRect ){ m_rect = textRect;}
	void clearTextPixmap(){ if ( m_labelPixmap.isNull() == false ) { m_labelPixmap = QPixmap(); } }

protected:
	GeoPoint m_coordinate;
	QPoint m_sympos;
	QPixmap m_symbolPixmap;
	QPixmap m_labelPixmap;
	QRect m_rect;
	int m_population;
	int m_selected;
	int m_symbol;
	int m_popidx;

	QChar m_role;
	QString m_description;
	QString m_countrycode;
};

#endif // PLACEMARK_H
