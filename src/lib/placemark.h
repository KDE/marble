//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef PLACEMARK_H
#define PLACEMARK_H


#include <QtCore/QChar>
#include <QtCore/QRect>
#include <QtCore/QVector>
#include <QtGui/QPixmap>

#include "GeoPoint.h"

class PlaceMark
{
 public:
    PlaceMark();
    PlaceMark( const QString& );

    GeoPoint  coordinate() const { return m_coordinate; } 
    void      coordinate( double &lon, double &lat );
    void      setCoordinate( double lon, double lat );

    const Quaternion& quaternion() const { return m_coordinate.quaternion(); }

    QString name() const         { return m_name; }
    void setName( QString name ) { m_name = name; }

    const QChar role() const   { return m_role; }
    void setRole( QChar role ) { m_role = role; }

    const QString description() const          { return m_description;        }
    void setDescription( QString description ) { m_description = description; }

    const QString countryCode() const          { return m_countrycode;        }
    void setCountryCode( QString countrycode ) { m_countrycode = countrycode; }

    const int symbol() const                   { return m_symbol;   }
    void setSymbol( int symbol )               { m_symbol = symbol; }

    const QPixmap  symbolPixmap() const;
    const QSize    symbolSize()   const;

    const int popidx() const                   { return m_popidx;   }
    void setPopidx( int popidx )               { m_popidx = popidx; }

    const int population() const               { return m_population;       }
    void setPopulation( int population )       { m_population = population; }

    const int selected() const                 { return m_selected;     }
    void setSelected( int selected )           { m_selected = selected; }


    QRect& textRect()              { return m_textRect;    }
    void setTextRect( const QRect& textRect ) { m_textRect = textRect;}
    QRect  m_textRect;
    const QPixmap& textPixmap() const          { return m_textPixmap;       }
    void setTextPixmap( QPixmap& textPixmap ) { m_textPixmap = textPixmap;}
    QPixmap m_textPixmap;
    void clearTextPixmap() {
        if ( !m_textPixmap.isNull() ) m_textPixmap = QPixmap(); 
    }
    QPoint& symbolPos()              { return m_symbolPos;   }
    void setSymbolPos( const QPoint& sympos )   { m_symbolPos = sympos; }
    QPoint      m_symbolPos;	// position of the placemark's symbol

 protected:
    GeoPoint  m_coordinate;     // The geographic location
    QString   m_name;

    // Basic data
    int       m_population;

    // View stuff
    int       m_selected;	// FIXME: Move to VisiblePlaceMark or the view
    int       m_symbol;
    int       m_popidx;

    QChar     m_role;
    QString   m_description;
    QString   m_countrycode;
};

#endif // PLACEMARK_H
