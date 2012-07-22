//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "VisiblePlacemark.h"

#include "MarbleDebug.h"

#include "GeoDataStyle.h"
#include "PlacemarkLayer.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QPixmap>

using namespace Marble;

VisiblePlacemark::VisiblePlacemark( const GeoDataPlacemark *placemark )
    : m_placemark( placemark ),
      m_selected( false )
{
    drawLabelPixmap();
}

const GeoDataPlacemark* VisiblePlacemark::placemark() const
{
    return m_placemark;
}

const QPixmap& VisiblePlacemark::symbolPixmap() const
{    
    GeoDataStyle* style = m_placemark->style();
    if ( style ) {
        m_symbolPixmap = QPixmap::fromImage( style->iconStyle().icon() );
    } else {
        mDebug() << "Style pointer null";
    }
    return  m_symbolPixmap;
}

bool VisiblePlacemark::selected() const
{
    return m_selected;
}

void VisiblePlacemark::setSelected( bool selected )
{
    m_selected = selected;
    drawLabelPixmap();
}

const QPoint& VisiblePlacemark::symbolPosition() const
{
    return m_symbolPosition;
}

void VisiblePlacemark::setSymbolPosition( const QPoint& position )
{
    m_symbolPosition = position;
}

const QPixmap& VisiblePlacemark::labelPixmap() const
{
    return m_labelPixmap;
}

const QRectF& VisiblePlacemark::labelRect() const
{
    return m_labelRect;
}

void VisiblePlacemark::setLabelRect( const QRectF& labelRect )
{
    m_labelRect = labelRect;
}

void VisiblePlacemark::drawLabelPixmap()
{

    QPainter labelPainter;

    const GeoDataStyle* style = m_placemark->style();

    QString labelName = m_placemark->name();
    if ( labelName.isEmpty() ) {
        m_labelPixmap = QPixmap();
        return;
    }

    QFont  labelFont  = style->labelStyle().font();
    QColor labelColor = style->labelStyle().color();

    LabelStyle labelStyle = Normal;
    if ( m_selected ) {
        labelStyle = Selected;
    } else if ( style->labelStyle().glow() ) {
        labelStyle = Glow;
    }

    int textHeight = QFontMetrics( labelFont ).height();

    int textWidth;
    if ( style->labelStyle().glow() ) {
        labelFont.setWeight( 75 ); // Needed to calculate the correct pixmap size;
        textWidth = ( QFontMetrics( labelFont ).width( labelName )
            + qRound( 2 * s_labelOutlineWidth ) );
    } else {
        textWidth = ( QFontMetrics( labelFont ).width( labelName ) );
    }


    // Due to some XOrg bug this requires a workaround via
    // QImage in some cases (at least with Qt 4.2).
    if ( !PlacemarkLayer::m_useXWorkaround ) {
        m_labelPixmap = QPixmap( QSize( textWidth, textHeight ) );
        m_labelPixmap.fill( Qt::transparent );

        labelPainter.begin( &m_labelPixmap );

        drawLabelText( labelPainter, labelName, labelFont, labelStyle, labelColor );

        labelPainter.end();
    } else {

        QImage image( QSize( textWidth, textHeight ),
                      QImage::Format_ARGB32_Premultiplied );
        image.fill( 0 );

        labelPainter.begin( &image );

        drawLabelText( labelPainter, labelName, labelFont, labelStyle, labelColor );

        labelPainter.end();

        m_labelPixmap = QPixmap::fromImage( image );
    }
}

void VisiblePlacemark::drawLabelText(QPainter &labelPainter, const QString &text,
                                            const QFont &labelFont, LabelStyle labelStyle, const QColor &color )
{
    QFont font = labelFont;
    QFontMetrics metrics = QFontMetrics( font );
    int fontAscent = metrics.ascent();

    switch ( labelStyle ) {
    case Selected: {
        labelPainter.setPen( color );
        labelPainter.setFont( font );
        QRect textRect( 0, 0, metrics.width( text ), metrics.height() );
        labelPainter.fillRect( textRect, QApplication::palette().highlight() );
        labelPainter.setPen( QPen( QApplication::palette().highlightedText(), 1 ) );
        labelPainter.drawText( 0, fontAscent, text );
        break;
    }
    case Glow: {
        font.setWeight( 75 );
        fontAscent = QFontMetrics( font ).ascent();

        QPen outlinepen( color == QColor( Qt::white ) ? Qt::black : Qt::white );
        outlinepen.setWidthF( s_labelOutlineWidth );
        QBrush  outlinebrush( color );

        QPainterPath outlinepath;

        const QPointF  baseline( s_labelOutlineWidth / 2.0, fontAscent );
        outlinepath.addText( baseline, font, text );
        labelPainter.setRenderHint( QPainter::Antialiasing, true );
        labelPainter.setPen( outlinepen );
        labelPainter.setBrush( outlinebrush );
        labelPainter.drawPath( outlinepath );
        labelPainter.setPen( Qt::NoPen );
        labelPainter.drawPath( outlinepath );
        labelPainter.setRenderHint( QPainter::Antialiasing, false );
        break;
    }
    default: {
        labelPainter.setPen( color );
        labelPainter.setFont( font );
        labelPainter.drawText( 0, fontAscent, text );
    }
    }
}

