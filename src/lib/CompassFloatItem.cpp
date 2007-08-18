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

#include "CompassFloatItem.h"

#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtSvg/QSvgRenderer>

#include "MarbleDirs.h"


CompassFloatItem::CompassFloatItem(QObject* parent)
    : QObject(parent)
{
    m_width = 0;

#ifdef Q_OS_MACX
    m_font = QFont( "Sans Serif", 10 );
#else
    m_font = QFont( "Sans Serif", 8 );
#endif

    m_font.setStyleStrategy( QFont::ForceOutline );

    m_fontheight = QFontMetrics( m_font ).ascent();
    m_fontwidth  = QFontMetrics( m_font ).boundingRect( "N" ).width();

    m_svgobj = new QSvgRenderer( MarbleDirs::path( "svg/compass.svg" ),
                                 this );

    m_polarity = 1;

    m_transparency = 192;
}

void CompassFloatItem::setSourceFile( const QString& relativePath )
{
    delete m_svgobj;
    m_svgobj = new QSvgRenderer( MarbleDirs::path( relativePath ),
                                 this );
    m_width = 0;
}

QPixmap& CompassFloatItem::drawCompassPixmap( int canvaswidth, int canvasheight, 
                                              int northpoley )
{
    int  width    = 52;
    int  polarity = 0;

    Q_UNUSED( canvaswidth );
    Q_UNUSED( canvasheight );

    if ( northpoley != 0 ) 
        polarity = northpoley / abs(northpoley);
    else
        polarity = 0;		

    if ( width == m_width && polarity == m_polarity )
        return m_pixmap;

    m_width    = width; 
    m_polarity = polarity;

    m_pixmap = QPixmap( m_width, m_width + m_fontheight + 5 );
    m_pixmap.fill( Qt::transparent );

    QPainter painter( &m_pixmap );

    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( Qt::black );

    painter.setBrush( QColor( 192, 192, 192, m_transparency ));


    QString dirstr = tr( "N" );
    if ( polarity == 1 ) 
        dirstr = tr( "S" );
    if ( polarity == 0 )
        dirstr = "";

    m_fontwidth = QFontMetrics( m_font ).boundingRect( dirstr ).width();

    QRect  viewport( 1, m_fontheight + 5 + 1, m_width - 1, m_width - 1 ); 
    painter.drawEllipse( QRectF( viewport ) );

    QPen outlinepen( QColor( 192, 192, 192, 192 ) );
    outlinepen.setWidth( 2 );
    QBrush outlinebrush( Qt::black );

    QPainterPath   outlinepath;
    const QPointF  baseline( 0.5 * (double)( m_width - m_fontwidth ),
                             (double)(m_fontheight) + 2.0 );
    outlinepath.addText( baseline, m_font, dirstr );

    painter.setPen( outlinepen );
    painter.setBrush( outlinebrush );
    painter.drawPath( outlinepath );

    painter.setPen( Qt::NoPen );
    painter.drawPath( outlinepath );

    painter.setViewport( viewport );
    m_svgobj->render( &painter ); 

    return m_pixmap;
}


#include "CompassFloatItem.moc"
