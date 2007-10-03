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

#include "PlaceMarkPainter.h"

#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPainter>

#include "ViewParams.h"
#include "VisiblePlaceMark.h"

PlaceMarkPainter::PlaceMarkPainter( QObject* parent )
    : QObject( parent )
{
#ifdef Q_OS_MACX
    m_font_regular           = QFont( "Sans Serif", 10, 50, false );
#else
    m_font_regular           = QFont( "Sans Serif",  8, 50, false );
#endif
    m_fontheight = QFontMetrics( m_font_regular ).height();
    m_fontascent = QFontMetrics( m_font_regular ).ascent();

    m_labelcolor = QColor( 0, 0, 0, 255 );

    m_useXWorkaround = testXBug();
    qDebug() << "Use workaround: " << ( m_useXWorkaround ? "1" : "0" );
}

PlaceMarkPainter::~PlaceMarkPainter()
{
}

void PlaceMarkPainter::setLabelColor( const QColor &color )
{
    m_labelcolor = color;
}

void PlaceMarkPainter::drawPlaceMarks( QPainter* painter, 
                                       QVector<VisiblePlaceMark*> visiblePlaceMarks,
                                       const QItemSelection &selection, 
                                       ViewParams *viewParams )
{
    QVector<VisiblePlaceMark*>::const_iterator visit = visiblePlaceMarks.constEnd();

    VisiblePlaceMark *mark = 0;
    int imageWidth = viewParams->m_canvasImage->width();

    switch( viewParams->m_projection ) {
        case Spherical:

            while ( visit != visiblePlaceMarks.constBegin() ) {
                --visit;
                mark = *visit;

                if ( mark->labelPixmap().isNull() )
                {
                    bool isSelected = selection.contains( mark->modelIndex() );
                    drawLabelPixmap( mark, isSelected );
                }

                painter->drawPixmap( mark->symbolPosition(), mark->symbolPixmap() );
                painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );
            }

            break;
        case Equirectangular:

            while ( visit != visiblePlaceMarks.constBegin() ) {
                --visit;
                mark = *visit;

                if ( mark->labelPixmap().isNull() )
                {
                    bool isSelected = selection.contains( mark->modelIndex() );
                    drawLabelPixmap( mark, isSelected );
                }

                painter->drawPixmap( mark->symbolPosition(), mark->symbolPixmap() );
                painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );

                int tempSymbol = mark->symbolPosition().x();
                int tempText =   mark->labelRect().x();

                for ( int i = tempSymbol - 4 * viewParams->m_radius;
                    i >= 0;
                    i -= 4 * viewParams->m_radius )
                {
                    QRect labelRect( mark->labelRect() );
                    labelRect.moveLeft(i - tempSymbol + tempText );
                    mark->setLabelRect( labelRect );

                    QPoint symbolPos( mark->symbolPosition() );
                    symbolPos.setX( i );
                    mark->setSymbolPosition( symbolPos );

                    painter->drawPixmap( mark->symbolPosition(), mark->symbolPixmap() );
                    painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );
                }

                for ( int i = tempSymbol;
                      i <= imageWidth;
                      i += 4 * viewParams->m_radius )
                {
                    QRect labelRect( mark->labelRect() );
                    labelRect.moveLeft(i - tempSymbol + tempText );
                    mark->setLabelRect( labelRect );

                    QPoint symbolPos( mark->symbolPosition() );
                    symbolPos.setX( i );
                    mark->setSymbolPosition( symbolPos );

                    painter->drawPixmap( mark->symbolPosition(), mark->symbolPixmap() );
                    painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );
                }
            }
            break;

        default:
            qDebug()<<"Projection not supported";
    }
}

inline void PlaceMarkPainter::drawLabelText(QPainter& labelPainter,
                                            const QString &name, const QFont &font )
{
    QPen    outlinepen( Qt::white );
    outlinepen.setWidthF( s_labelOutlineWidth );
    QBrush  outlinebrush( Qt::black );

    QPainterPath   outlinepath;
    const QPointF  baseline( s_labelOutlineWidth / 2.0, m_fontascent );
    outlinepath.addText( baseline, font, name );
    labelPainter.setRenderHint( QPainter::Antialiasing, true );
    labelPainter.setPen( outlinepen );
    labelPainter.setBrush( outlinebrush );
    labelPainter.drawPath( outlinepath );
    labelPainter.setPen( Qt::NoPen );
    labelPainter.drawPath( outlinepath );
    labelPainter.setRenderHint( QPainter::Antialiasing, false );
}

inline void PlaceMarkPainter::drawLabelPixmap( VisiblePlaceMark *mark, bool isSelected )
{

    QPainter labelPainter;
    QPixmap labelPixmap;

    // Due to some XOrg bug this requires a workaround via
    // QImage in some cases.
    if ( !m_useXWorkaround ) {
        labelPixmap = QPixmap( mark->labelRect().width(), m_fontheight );
        labelPixmap.fill( Qt::transparent );

        labelPainter.begin( &labelPixmap );

        if ( !isSelected ) {
            labelPainter.setFont( mark->labelFont() );
            labelPainter.setPen( m_labelcolor );
            labelPainter.drawText( 0, m_fontascent, mark->name() );
        }
        else {
            drawLabelText( labelPainter, mark->name(), mark->labelFont() );
        }

        labelPainter.end();
    } else {

        QImage image( mark->labelRect().width(), m_fontheight,
                      QImage::Format_ARGB32_Premultiplied );
        image.fill( 0 );

        labelPainter.begin( &image );

        if ( !isSelected ) {
            labelPainter.setFont( mark->labelFont() );
            labelPainter.setPen( m_labelcolor );
            labelPainter.drawText( 0, m_fontascent, mark->name() );
        }
        else {
            drawLabelText( labelPainter, mark->name(), mark->labelFont() );
        }

        labelPainter.end();

        labelPixmap = QPixmap::fromImage( image );
    }

    mark->setLabelPixmap( labelPixmap );
}


// Test if there a bug in the X server which makes 
// text fully transparent if it gets written on 
// QPixmaps that were initialized by filling them 
// with Qt::transparent

bool PlaceMarkPainter::testXBug()
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

    for ( int x = 0; x < fontwidth; x++ ) {
        for ( int y = 0; y < fontheight; y++ ) {
            if ( qAlpha( image.pixel( x, y ) ) > 0 )
                return false;
        }
    }

    return true;
}

#include "PlaceMarkPainter.moc"

