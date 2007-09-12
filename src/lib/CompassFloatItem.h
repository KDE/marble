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
// CompassFloatItem paints a compass on a pixmap
//


#ifndef COMPASSFLOATITEM_H
#define COMPASSFLOATITEM_H


#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QFont>


class QSvgRenderer;

class CompassFloatItem : public QObject
{
    Q_OBJECT

 public:
    CompassFloatItem( QObject *parent = 0 );
    ~CompassFloatItem();

    QPixmap  &drawCompassPixmap( int, int, int );

    int transparency() const { return m_transparency; }
    void setTransparency( int transparency ) { m_transparency = transparency; m_width = -1; }

    void setSourceFile( const QString& relativePath );

 protected:

    QSvgRenderer  *m_svgobj;
    QPixmap        m_pixmap;
    int            m_width;

    QFont          m_font;
    int            m_fontwidth;
    int            m_fontheight;
    int            m_polarity;

    int m_transparency;
};


#endif // COMPASSFLOATITEM_H
