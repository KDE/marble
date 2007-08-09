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
#include "PlaceMarkContainer.h"


class QPainter;
class PlaceMarkContainer;
class VisiblePlaceMark;
class ViewParams;


class PlaceMarkPainter : public QObject
{
    Q_OBJECT

 public:
    PlaceMarkPainter(QObject *parent = 0);

    void paintPlaceFolder(QPainter*, int imgwidth, int imgheight,
                          ViewParams *viewParams,
                          const PlaceMarkContainer*,
                          Quaternion,
                          bool firstTime = true );

    void setLabelColor(QColor labelcolor) { m_labelcolor = labelcolor; }
    QVector<PlaceMark*> whichPlaceMarkAt( const QPoint& );

 private:
    void sphericalPaintPlaceFolder(QPainter*, int imgwidth, int imgheight,
                          ViewParams *viewParams,
                          const PlaceMarkContainer*,
                          Quaternion,
                          bool firstTime );
    void rectangularPaintPlaceFolder(QPainter*, int imgwidth, int imgheight,
                          ViewParams *viewParams,
                          const PlaceMarkContainer*,
                          Quaternion,
                          bool firstTime );

    bool   isVisible( PlaceMark *mark, int radius,
                          Quaternion &rotAxis,
                          int imgwidth, int imgheight,
                          ViewParams *viewParams,
                          int &x, int &y  );
    bool   roomForLabel( const QVector<VisiblePlaceMark*> &currentsec,
                          VisiblePlaceMark *visibleMark,
                          PlaceMark *mark,
                          int textwidth,
                          int x, int y );
    void drawLabelText( QPainter& textpainter, PlaceMark*, QFont font,
                          double outlineWidth );
    bool testbug();

 protected:

    QFont  m_font_regular;
    QFont  m_font_regular_italics;
    QFont  m_font_regular_underline;
    QFont  m_font_mountain;

    // All the visible placemarks.  FIXME: Move to the view.
#if 0
    QList<VisiblePlaceMark*>    m_visiblePlacemarks;
#else
    QVector<PlaceMark*>    m_visiblePlacemarks;
#endif
    QVector<VisiblePlaceMark*>  m_visiblePlacemarksPool;

    QColor  m_labelcolor;
    int     m_fontheight;
    int     m_fontascent;
    int     m_labelareaheight;

    // QVector< QPixmap > m_citysymbol;
    QVector< int >        m_weightfilter;
    QPixmap  m_empty;
    double   m_widthscale;

    bool     m_useworkaround;
};


#endif // PLACEMARKPAINTER_H
