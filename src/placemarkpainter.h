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

//
// PlaceMarkPainter is responsible for drawing the PlaceMarks on the map
//


#ifndef PLACEMARKPAINTER_H
#define PLACEMARKPAINTER_H


#include <QtGui/QFont>
#include <QtGui/QPainterPath>
#include <QtGui/QPixmap>
#include <QtCore/QRect>
#include <QtCore/QVector>

#include "Quaternion.h"
#include "placemark.h"
#include "placecontainer.h"


/**
@author Torsten Rahn
*/

class QAbstractItemModel;
class QPainter;
class PlaceContainer;

class PlaceMarkPainter : public QObject
{
    Q_OBJECT

 public:
    PlaceMarkPainter(QObject *parent = 0);
    void paintPlaceFolder(QPainter*, int, int, int, const PlaceContainer*, 
                          Quaternion );
    void setLabelColor(QColor labelcolor){ m_labelcolor = labelcolor;}
    QVector<PlaceMark*> whichPlaceMarkAt( const QPoint& );

 public slots:

 protected:
    inline void drawLabelText(QPainter& textpainter, PlaceMark*, QFont font, 
                              float outlineWidth);
    bool testbug(); 

 protected:

    QFont  m_font_regular;
    QFont  m_font_regular_italics;
    QFont  m_font_regular_underline;
    QFont  m_font_mountain;

    PlaceContainer  m_visibleplacemarks;

    QColor  m_labelcolor;
    int     m_fontheight;
    int     m_fontascent;
    int     m_labelareaheight;

    // QVector< QPixmap > m_citysymbol;
    QVector< int >        m_weightfilter;
    QPixmap  m_empty;
    float    m_widthscale;

    bool     m_useworkaround;
};


#endif // PLACEMARKPAINTER_H
