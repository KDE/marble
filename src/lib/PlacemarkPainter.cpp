//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//

#include "PlacemarkPainter.h"

#include <QtCore/QModelIndex>
#include <QtCore/QPoint>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>

#include "MarbleDebug.h"
#include "AbstractProjection.h"
#include "GeoDataStyle.h"
#include "MarblePlacemarkModel.h"
#include "ViewportParams.h"
#include "VisiblePlacemark.h"
#include <QPalette>
#include <QApplication>

using namespace Marble;

PlacemarkPainter::PlacemarkPainter( QObject* parent )
    : QObject( parent )
{
    m_useXWorkaround = testXBug();
    mDebug() << "Use workaround: " << ( m_useXWorkaround ? "1" : "0" );

    m_defaultLabelColor = Qt::black;
}

PlacemarkPainter::~PlacemarkPainter()
{
}

void PlacemarkPainter::setDefaultLabelColor( const QColor& color )
{
    m_defaultLabelColor = color;
}

void PlacemarkPainter::drawPlacemarks( QPainter* painter, 
                                       QVector<VisiblePlacemark*> visiblePlacemarks,
                                       ViewportParams *viewport )
{
    int imageWidth = viewport->width();

    foreach( VisiblePlacemark *mark, visiblePlacemarks ) {
	if ( mark->labelPixmap().isNull() ) {
            drawLabelPixmap( mark );
	}

        int symbolX = mark->symbolPosition().x();
        int textX =   mark->labelRect().x();
        QRect labelRect( mark->labelRect() );
        QPoint symbolPos( mark->symbolPosition() );

        // when the map is such zoomed out that a given place
        // appears many times, we draw one placemark at each
        if (viewport->currentProjection()->repeatX() ) {
            for ( int i = symbolX % (4 * viewport->radius());
                 i <= imageWidth;
                 i += 4 * viewport->radius() )
            {
                labelRect.moveLeft(i - symbolX + textX );
                symbolPos.setX( i );

                painter->drawPixmap( symbolPos, mark->symbolPixmap() );
                painter->drawPixmap( labelRect, mark->labelPixmap() );
            }
        } else { // simple case, one draw per placemark
            painter->drawPixmap( symbolPos, mark->symbolPixmap() );
            painter->drawPixmap( labelRect, mark->labelPixmap() );
        }
    }
}

inline void PlacemarkPainter::drawLabelText(QPainter &labelPainter, const QString &text,
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

inline void PlacemarkPainter::drawLabelPixmap( VisiblePlacemark *mark )
{

    QPainter labelPainter;
    QPixmap labelPixmap;

    const GeoDataPlacemark *placemark = mark->placemark();
    Q_ASSERT(placemark);
    const GeoDataStyle* style = placemark->style();

    QString labelName = placemark->name();
    QRect  labelRect  = mark->labelRect();
    if ( !labelRect.isValid() ) {
        mark->setLabelPixmap( QPixmap() );
        return;
    }
    
    QFont  labelFont  = style->labelStyle().font();
    QColor labelColor = style->labelStyle().color();

    // FIXME: To be removed after MapTheme / KML refactoring
    if ( ( labelColor == Qt::black || labelColor == QColor( "#404040" ) )
	 && m_defaultLabelColor != Qt::black )
        labelColor = m_defaultLabelColor;

    LabelStyle labelStyle = Normal;
    if ( mark->selected() ) {
        labelStyle = Selected;
    } else if ( style->labelStyle().glow() ) {
        labelFont.setWeight(75);
        labelStyle = Glow;
    }


    // Due to some XOrg bug this requires a workaround via
    // QImage in some cases (at least with Qt 4.2).
    if ( !m_useXWorkaround ) {
        labelPixmap = QPixmap( labelRect.size() );
        labelPixmap.fill( Qt::transparent );

        labelPainter.begin( &labelPixmap );

        drawLabelText( labelPainter, labelName, labelFont, labelStyle, labelColor );

        labelPainter.end();
    } else {

        QImage image( labelRect.size(),
                      QImage::Format_ARGB32_Premultiplied );
        image.fill( 0 );

        labelPainter.begin( &image );

        drawLabelText( labelPainter, labelName, labelFont, labelStyle, labelColor );

        labelPainter.end();

        labelPixmap = QPixmap::fromImage( image );
    }

    mark->setLabelPixmap( labelPixmap );
}


// Test if there a bug in the X server which makes 
// text fully transparent if it gets written on 
// QPixmaps that were initialized by filling them 
// with Qt::transparent

bool PlacemarkPainter::testXBug()
{
    QString  testchar( "K" );
    QFont    font( "Sans Serif", 10 );

    int fontheight = QFontMetrics( font ).height();
    int fontwidth  = QFontMetrics( font ).width(testchar);
    int fontascent = QFontMetrics( font ).ascent();

    QPixmap  pixmap( fontwidth, fontheight );
    pixmap.fill( Qt::transparent );

    QPainter textpainter;
    textpainter.begin( &pixmap );
    textpainter.setPen( QColor( 0, 0, 0, 255 ) );
    textpainter.setFont( font );
    textpainter.drawText( 0, fontascent, testchar );
    textpainter.end();

    QImage image = pixmap.toImage();

    for ( int x = 0; x < fontwidth; ++x ) {
        for ( int y = 0; y < fontheight; ++y ) {
            if ( qAlpha( image.pixel( x, y ) ) > 0 )
                return false;
        }
    }

    return true;
}

#include "PlacemarkPainter.moc"

