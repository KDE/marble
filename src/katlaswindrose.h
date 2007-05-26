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
// KAtlasWindRose paints a windrose on a pixmap
//


#ifndef KATLASWINDROSE_H
#define KATLASWINDROSE_H


#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QFont>


/**
@author Torsten Rahn
*/

class QSvgRenderer;

class KAtlasWindRose : public QObject
{
    Q_OBJECT

 public:
    KAtlasWindRose( QObject *parent = 0 );

    QPixmap  &drawWindRosePixmap( int, int, int );

 protected:

    QSvgRenderer  *m_svgobj;
    QPixmap        m_pixmap;
    int            m_width;

    QFont          m_font;
    int            m_fontwidth;
    int            m_fontheight;
    int            m_polarity;
};


#endif // KATLASWINDROSE_H
