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

#include "placemarkpainter.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QVectorIterator>
#include <QtGui/QIcon>
#include <QtGui/QPainter>

#include "placemark.h"
#include "katlasdirs.h"


static const int  maxlabels = 100;

PlaceMarkPainter::PlaceMarkPainter(QObject* parent)
    : QObject(parent)
{
#ifdef Q_OS_MACX
    m_font_mountain = QFont( "Sans Serif",9, 50, false );

    m_font_regular           = QFont( "Sans Serif", 10, 50, false );
    m_font_regular_italics   = QFont( "Sans Serif", 10, 50, true );
    m_font_regular_underline = QFont( "Sans Serif", 10, 50, false );
#else
    m_font_mountain          = QFont( "Sans Serif",  7, 50, false );

    m_font_regular           = QFont( "Sans Serif",  8, 50, false );
    m_font_regular_italics   = QFont( "Sans Serif",  8, 50, true );
    m_font_regular_underline = QFont( "Sans Serif",  8, 50, false );
#endif

    m_font_regular_underline.setUnderline( true );

    m_fontheight = QFontMetrics( m_font_regular ).height();
    m_fontascent = QFontMetrics( m_font_regular ).ascent();

    m_labelareaheight = 2 * m_fontheight;

    m_labelcolor = QColor( 0, 0, 0, 255 );

    m_weightfilter
        << 9999
        << 4200
        << 3900
        << 3600

        << 3300
        << 3000
        << 2700
        << 2400

        << 2100
        << 1800
        << 1500
        << 1200

        << 900
        << 400
        << 200
        << 0;

    m_showCities = true;
    m_showTerrain = true;

    m_useworkaround = testbug();
    // m_useworkaround = true;
}

void PlaceMarkPainter::paintPlaceFolder(QPainter* painter, 
                                        int imgrx, int imgry,
                                        int radius,
                                        const PlaceMarkContainer* placeMarkContainer,
                                        Quaternion rotAxis )
{
    int  imgwidth  = 2 * imgrx;
    int  imgheight = 2 * imgry;
    int  x = 0;
    int  y = 0; 

    int  secnumber = imgheight / m_labelareaheight + 1;
#if 0
      if ( mark->name().contains( "London" ) ){
      qDebug() << "London" << " y: " << QString::number( y ) << " qpos.v[Q_Y]: " << QString::number( qpos.v[Q_Y] );
      invrotAxis.display();
      }
#endif
    // rotAxis.display();

    Quaternion  invRotAxis = rotAxis.inverse();
    Quaternion  qpos;

    painter->setPen(QColor(Qt::black));	

    QPainter    textpainter;

    QFont       font;

    const double outlineWidth = 2.5;
    int          fontwidth = 0;

    QPixmap     textpixmap;

    QVector< QVector< PlaceMark* > >  m_rowsection;
    for ( int i = 0; i < secnumber; i++)
        m_rowsection.append( QVector<PlaceMark*>( ) );

    m_visibleplacemarks.clear();

    PlaceMark  *mark = 0; 
    int         labelnum = 0;

    PlaceMarkContainer::const_iterator  it;
    for ( it = placeMarkContainer->constBegin();
          it != placeMarkContainer->constEnd();
          it++ ) 
    {
        mark  = *it; // no cast

        // Skip the places that are too small.
        if ( m_weightfilter.at( mark->popidx() ) > radius
//             && mark->symbol() != 0
             && mark->selected() == 0 )
            continue;

        // Skip terrain marks if we're not showing terrain.
        if ( m_showTerrain == false
             && ( mark->symbol() >= 16 && mark->symbol() <= 19 ) )
            continue;

        // Skip city marks if we're not showing cities.
        if ( m_showCities == false
             && ( mark->symbol() >= 0 && mark->symbol() < 16 ) )
            continue;

        qpos = mark->quaternion();

        qpos.rotateAroundAxis(invRotAxis);

        textpixmap = mark->textPixmap();

        if ( qpos.v[Q_Z] > 0 ) {

            x = (int)(imgrx + radius * qpos.v[Q_X]);
            y = (int)(imgry + radius * qpos.v[Q_Y]);

            // Don't process placemarks if they are outside the screen area
            if ( x >= 0 && x < imgwidth && y >= 0 && y < imgheight ) {

                // Choose Section
                const QVector<PlaceMark*>  currentsec = m_rowsection.at( y / m_labelareaheight ); 

                // Specify font properties
                if ( textpixmap.isNull() ) {

                    QChar  role = mark->role();

                    // C: Admin. center of _C_ountry
                    // R: Admin. center of _R_egion
                    // B: Admin. center of country and region ("_B_oth")
                    // N: _N_one

                    if ( role == 'N' ) { 
                        font = m_font_regular;
                    } else if ( role == 'R' ) {
                        font = m_font_regular_italics;
                    } else if ( role == 'B' || mark->role() == 'C' ) {
                        font = m_font_regular_underline;
                    } else {
                        font = m_font_regular;
                    }

                    if ( mark->symbol() > 13 || mark->selected() != 0 )
                        font.setWeight( 75 );

                    if ( role == 'P' ) 
                        font = m_font_regular;
                    if ( role == 'M' ) 
                        font = m_font_regular;
                    if ( role == 'H' ) 
                        font = m_font_mountain;
                    if ( role == 'V' ) 
                        font = m_font_mountain;

                    fontwidth = ( QFontMetrics( font ).width( mark->name() )
                                  + (int)( outlineWidth ) );
                }
                else{
                    fontwidth = ( mark->textRect() ).width();
                }

                // Find out whether the area around the placemark is
                // covered already.

                bool  overlap     = true;
                int   symbolwidth = mark->symbolSize().width();

                int  xpos = symbolwidth / 2 + x + 1;
                int  ypos = 0;

                while ( xpos >= x - fontwidth - symbolwidth - 1
                        && overlap == true )
                {
                    ypos = y; 
	
                    while ( ypos >= y - m_fontheight && overlap == true) { 

                        overlap = false;

                        QRect  textRect( xpos, ypos, fontwidth, m_fontheight );


                        for ( QVector<PlaceMark*>::const_iterator beforeit = currentsec.constBegin();
                              beforeit != currentsec.constEnd();
                              beforeit++ )
                        {
                            if ( textRect.intersects( (*beforeit)->textRect()) ) {
                                overlap = true;
                                break;
                            }
                        }
					
                        if ( overlap == false ) {
                            mark->setTextRect( textRect );
                        }
                        ypos -= m_fontheight; 
                    }

                    xpos -= ( symbolwidth + fontwidth + 2 );
                }

                // Paint the label
                if ( overlap == false) {
                    if ( textpixmap.isNull() == true ) {
                        // Draw the text on the label.

                        // Due to some XOrg bug this requires a
                        // workaround via QImage in some cases.

                        if ( !m_useworkaround ) {
                            textpixmap = QPixmap( fontwidth, m_fontheight );
                            textpixmap.fill(Qt::transparent);

                            textpainter.begin( &textpixmap );

                            if ( mark->selected() == 0 ) {
                                textpainter.setFont(font);
                                textpainter.setPen(m_labelcolor);
                                textpainter.drawText( 0, m_fontascent, mark->name() );
                            }
                            else {
                                drawLabelText(textpainter, mark, font, outlineWidth);
                            }

                            textpainter.end();
                        }
                        else {
                            QImage textimage( fontwidth, m_fontheight,
                                              QImage::Format_ARGB32_Premultiplied );
                            textimage.fill( 0 );

                            textpainter.begin( &textimage );

                            if ( mark->selected() == 0 ) {
                                textpainter.setFont(font);
                                textpainter.setPen(m_labelcolor);
                                textpainter.drawText( 0, m_fontascent, mark->name() );
                            }
                            else {
                                drawLabelText(textpainter, mark, font, outlineWidth);
                            }

                            textpainter.end();

                            textpixmap = QPixmap::fromImage( textimage );
                        }

                        mark->setTextPixmap( textpixmap );
                    }

                    // Finally save the label position on the map.
                    mark->setSymbolPos( QPoint( x - mark->symbolSize().width()  / 2,
                                                y - mark->symbolSize().height() / 2) );

                    // Add the current placemark to the matching row and it's
                    // direct neighbors.
                    int idx = y / m_labelareaheight;
                    if ( idx - 1 >= 0 )
                        m_rowsection[ idx - 1 ].append( mark );
                    m_rowsection[ idx ].append( mark );
                    if ( idx + 1 < secnumber )
                        m_rowsection[ idx + 1 ].append( mark );

                    m_visibleplacemarks.append(mark);
                    labelnum ++;
                    if ( labelnum >= maxlabels )
                        break;				
                }
            }
            else{
                mark->clearTextPixmap();
            }
        }
        else {
            mark->clearTextPixmap();
        }
    }

    // qDebug() << QString("Size: %1, Rows: %2").arg(m_visibleplacemarks.size()).arg( secnumber );
    PlaceMarkContainer::const_iterator  visit = m_visibleplacemarks.constEnd();

    while ( visit != m_visibleplacemarks.constBegin() ) {
        --visit;
        mark = *visit;
        painter->drawPixmap( mark->textRect(),  mark->textPixmap() );
        painter->drawPixmap( mark->symbolPos(), mark->symbolPixmap() );
    }
}


inline void PlaceMarkPainter::drawLabelText(QPainter& textpainter, 
                                            PlaceMark* mark, QFont font,
                                            double outlineWidth)
{
    QPen    outlinepen( Qt::white );
    outlinepen.setWidthF( outlineWidth );
    QBrush  outlinebrush( Qt::black );

    QPainterPath   outlinepath;
    const QPointF  baseline( outlineWidth / 2.0, m_fontascent );
    outlinepath.addText( baseline, font, mark->name() );
    textpainter.setRenderHint( QPainter::Antialiasing, true );
    textpainter.setPen( outlinepen );
    textpainter.setBrush( outlinebrush );
    textpainter.drawPath( outlinepath );
    textpainter.setPen( Qt::NoPen );
    textpainter.drawPath( outlinepath );
    textpainter.setRenderHint( QPainter::Antialiasing, false );

#if 0
    // Debug stuff:
    QString str = mark->name();
    qDebug() << str;
    QChar *data = str.data();
    int len = str.length();
    while (len != 0) {
    qDebug() << data->unicode();
    ++data;
    --len;
    }
#endif
}

bool PlaceMarkPainter::testbug()
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

    for ( int x = 0; x < fontwidth; x++ )
        for ( int y = 0; y < fontheight; y++ ) {
            if ( qAlpha( image.pixel( x,y ) ) > 0 )
                return false;
        }

    return true;
}


QVector<PlaceMark*> PlaceMarkPainter::whichPlaceMarkAt( const QPoint& curpos )
{
    QVector<PlaceMark*>             ret;

    PlaceMarkContainer::const_iterator  it;

    for ( it = m_visibleplacemarks.constBegin();
          it != m_visibleplacemarks.constEnd();
          it++ )
    {
        PlaceMark  *mark = *it; // no cast

        if ( mark->textRect().contains( curpos )
             || QRect( mark->symbolPos(), mark->symbolSize() ).contains( curpos ) ) {
            ret.append( mark );
        }
    }

    return ret;
}


#include "placemarkpainter.moc"
