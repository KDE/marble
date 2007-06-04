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

#include "katlasmapscale.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QColor>


const double earthdiameter = 6378.0; // km


KAtlasMapScale::KAtlasMapScale( QObject* parent )
    : QObject(parent)
{
    m_unit = "km";
    m_scalebarwidth = 250;
    m_scalebarheight = 5;

#ifdef Q_OS_MACX
    m_font = QFont( "Sans Serif", 10 );
#else
    m_font = QFont( "Sans Serif", 8 );
#endif
    m_fontheight  = QFontMetrics( m_font ).ascent();
    m_leftmargin  = QFontMetrics( m_font ).boundingRect( "0" ).width() / 2;
    m_rightmargin = QFontMetrics( m_font ).boundingRect( "0000" ).width() / 2;

    m_invscale      = 0;
    m_radius        = 0;
    m_scalebarkm    = 0.0;
    m_bestdivisor   = 0;
    m_pixelinterval = 0;
    m_valueinterval = 0;
}


QPixmap& KAtlasMapScale::drawScaleBarPixmap( int radius, int width )
{
    if ( radius == m_radius
         && m_scalebarwidth == width - m_leftmargin - m_rightmargin )
        return m_pixmap;
    m_radius = radius;

    m_pixmap = QPixmap( width + 20, m_fontheight + m_scalebarheight + 20 );
    m_pixmap.fill( Qt::transparent );

    QPainter painter( &m_pixmap );
	
    painter.setPen( QColor( Qt::black ) );
    painter.setBrush( QColor( 192, 192, 192, 192 ) );

    painter.translate( 0.5, 0.5 );
    painter.drawRect( 0, 0, m_pixmap.width() - 1, m_pixmap.height() - 1 );
    painter.translate( 4.5, 4.5 );

    paintScaleBar( &painter, radius, width );	
    return m_pixmap;
}


void KAtlasMapScale::paintScaleBar( QPainter* painter, int radius, int width )
{

    if ( radius == m_radius && width == m_scalebarwidth )
        return;

    m_radius        = radius;
    m_scalebarwidth = width - m_leftmargin - m_rightmargin;

    m_scalebarkm = (double)(m_scalebarwidth) * earthdiameter / (double)(radius);

    calcScaleBar();

    // painter->setRenderHint(QPainter::TextAntialiasing, false);
    painter->setFont( m_font );

    painter->setPen( QColor( Qt::darkGray ) );
    painter->setBrush( QColor( Qt::darkGray ) );
    painter->drawRect( m_leftmargin, m_fontheight + 3,
                       m_leftmargin + m_scalebarwidth + m_rightmargin,
                       m_scalebarheight );

    painter->setPen( QColor( Qt::black ) );
    painter->setBrush( QColor( Qt::white ) );
    painter->drawRect( m_leftmargin, m_fontheight + 3,
                       m_bestdivisor * m_pixelinterval, m_scalebarheight );

    painter->setBrush( QColor( Qt::black ) );

    QString  intervalstr;
    int      lastStringEnds     = 0;
    int      currentStringBegin = 0;
 
    for ( int j = 0; j <= m_bestdivisor; j += 2 ) {
        if ( j < m_bestdivisor )
            painter->drawRect( m_leftmargin + j * m_pixelinterval,
                               m_fontheight + 3, m_pixelinterval - 1,
                               m_scalebarheight );

        intervalstr.setNum( j * m_valueinterval );

        if ( j == 0 ) {
            painter->drawText( 0, m_fontheight, "0 " + m_unit );
            lastStringEnds = QFontMetrics( m_font ).width( "0 " + m_unit );
            continue;
        }

        currentStringBegin = ( m_leftmargin + j * m_pixelinterval 
                               - QFontMetrics( m_font ).width( intervalstr ) / 2 );
        if ( lastStringEnds < currentStringBegin ) {
            painter->drawText( currentStringBegin, m_fontheight, intervalstr );
            lastStringEnds = currentStringBegin + QFontMetrics( m_font ).width( intervalstr );
        }
    }
}


void KAtlasMapScale::calcScaleBar()
{
    double  magnitude = 1;

    // First we calculate the exact length of the whole area that is possibly 
    // available scale bar available to the scalebar in kilometers
    int  magvalue = (int)( m_scalebarkm );

    // We calculate the two most significant digits of the km-scalebar-length
    // and store them in magvalue.
    while ( magvalue >= 100 ) {
        magvalue  /= 10;
        magnitude *= 10; 
    }

    m_bestdivisor = 4;
    int  bestmagvalue = 1;

    for ( int i = 0; i < magvalue; i++ ) {
        // We try to find the lowest divisor between 4 and 8 that
        // divides magvalue without remainder. 
        for ( int j = 4; j < 9; j++ ) {
            if ( ( magvalue - i ) % j == 0 ) {
                // We store the very first result we find and store
                // m_bestdivisor and bestmagvalue as a final result.
                m_bestdivisor = j;
                bestmagvalue  = magvalue - i;

                // Stop all for loops and end search
                i = magvalue; 
                j = 9;
            }
        }

        // If magvalue doesn't divide through values between 4 and 8
        // (e.g. because it's a prime number) try again with magvalue
        // decreased by i.
    }

    m_pixelinterval = (int)( m_scalebarwidth * (double)( bestmagvalue )
                             / (double)( magvalue ) / m_bestdivisor );
    m_valueinterval = (int)( bestmagvalue * magnitude / m_bestdivisor );
}


#include "katlasmapscale.moc"
