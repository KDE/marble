//
// C++ Interface: gpmapscale
//
// Description: KAtlasMapScale 

// KAtlasMapScale resembles the scale in terms of value and visualization
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution


#ifndef KATLASMAPSCALE_H
#define KATLASMAPSCALE_H


#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>


/**
@author Torsten Rahn
*/


class KAtlasMapScale : public QObject
{
    Q_OBJECT

 public:
    KAtlasMapScale(QObject *parent = 0);

    int   invScale() const            { return m_invscale; }
    void  setInvScale( int invscale ) { m_invscale = invscale; }

    int   scaleBarHeight() const      { return m_scalebarheight; }
    void  setScaleBarHeight( int scalebarheight ) {
        m_scalebarheight = scalebarheight;
    }

    int scaleBarWidth()const{ return m_scalebarwidth; }

    void      paintScaleBar( QPainter*, int, int );
    QPixmap&  drawScaleBarPixmap( int, int );

 public slots:
    void setScaleBarWidth( int scalebarwidth ) {
        m_scalebarwidth = scalebarwidth;
    }

 private:
    void calcScaleBar();

 private:
    QPixmap  m_pixmap;
    int      m_invscale;
    int      m_radius;
    int      m_scalebarwidth;
    int      m_scalebarheight;
    double   m_scalebarkm;

    QFont    m_font;
    int      m_leftmargin;
    int      m_rightmargin;
    int      m_fontheight;
    int      m_bestdivisor;
    int      m_pixelinterval;
    int      m_valueinterval;

    QString  m_unit;
};


#endif // KATLASMAPSCALE_H
