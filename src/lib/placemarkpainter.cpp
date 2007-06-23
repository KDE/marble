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
#include <QtCore/QVector>
#include <QtCore/QVectorIterator>
#include <QtGui/QIcon>
#include <QtGui/QPainter>

#include "placemark.h"
#include "katlasdirs.h"


// ================================================================
//                     class VisiblePlaceMark


class VisiblePlaceMark
{
 public:
    VisiblePlaceMark( PlaceMark *mark );
    ~VisiblePlaceMark();

    PlaceMark *placeMark() const                { return m_placeMark;  }
    void setPlaceMark( PlaceMark *_mark )       { m_placeMark = _mark; }

    const QPixmap  symbolPixmap() const { return m_placeMark->symbolPixmap();};
    const QSize    symbolSize()   const { return m_placeMark->symbolSize();  };

    const QPoint& symbolPos() const             { return m_symbolPos;   }
    void setSymbolPos( const QPoint& sympos )   { m_symbolPos = sympos; }
    void setSymbolPos( const int x, const int y ) {
	m_symbolPos = QPoint( x, y );
    }

    const QPixmap& labelPixmap() const          { return m_labelPixmap;       }
    void setLabelPixmap( QPixmap& labelPixmap ) { m_labelPixmap = labelPixmap;}

    void clearLabelPixmap() {
        if ( m_labelPixmap.isNull() == false )
            m_labelPixmap = QPixmap(); 
    }

    const QRect& labelRect() const              { return m_labelRect;    }
    void setLabelRect( const QRect& labelRect ) { m_labelRect = labelRect;}

 private:
    PlaceMark  *m_placeMark;	// the actual PlaceMark

    // View stuff
    QPoint      m_symbolPos;	// position of the placemark's symbol
    QPixmap     m_labelPixmap;	// the text label (most often name)
    QRect       m_labelRect;    // bounding box of label
};


VisiblePlaceMark::VisiblePlaceMark( PlaceMark *mark )
    : m_symbolPos(),
      m_labelPixmap(),
      m_labelRect()
{
    m_placeMark = mark;
}


VisiblePlaceMark::~VisiblePlaceMark()
{
}


// ================================================================
//                     class PlaceMarkPainter


static const int  maxlabels = 100;

PlaceMarkPainter::PlaceMarkPainter(QObject* parent)
    : QObject(parent),
      m_visiblePlacemarks(),
      m_visiblePlacemarksPool()
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
    qDebug() << "Use workaround: " << ( m_useworkaround ? "1" : "0" );
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
    int          textwidth = 0;

    QPixmap     textpixmap;

    // Introduce a QVector of QVectors.  Each QVector contains a
    // pointer to the VisiblePlaceMarks that have their names visible
    // on that row.  A "row" is not a pixel row, but a row of names
    // that are overlaid the picture.  Each row is one fontHeight()
    // pixel high.  This is later used for collision detection for the
    // labels.
    QVector< QVector< VisiblePlaceMark* > >  rowsection;
    for ( int i = 0; i < secnumber; i++)
        rowsection.append( QVector<VisiblePlaceMark*>( ) );

    // Move all visible placemarks to the pool for later use.
    for ( QVector<VisiblePlaceMark*>::const_iterator it = m_visiblePlacemarks.constBegin();
	  it != m_visiblePlacemarks.constEnd();
	  ++it )
    {
	m_visiblePlacemarksPool.append( *it );
    }
    m_visiblePlacemarks.clear();

    PlaceMark         *mark = 0; 
    VisiblePlaceMark  *visibleMark = 0; 
    int                labelnum = 0;

    // Loop through ALL PlaceMarks and get those that are visible.
    // All the visible ones are collected into a QVector of
    // VisiblePlaceMarks.
    PlaceMarkContainer::const_iterator  it2;
    for ( it2 = placeMarkContainer->constBegin();
          it2 != placeMarkContainer->constEnd();
          ++it2 ) 
    {
        mark  = *it2; // no cast

        // Skip the places that are too small and not selected.
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

	// FIXME: optimize away this one
	textpixmap = QPixmap();

        // Skip the place if it's on the other side of the globe.
        if ( qpos.v[Q_Z] < 0 ) {
            continue;
        }

        x = (int)(imgrx + radius * qpos.v[Q_X]);
        y = (int)(imgry + radius * qpos.v[Q_Y]);

        // Don't process placemarks if they are outside the screen area
        if ( x < 0 || x >= imgwidth || y < 0 || y >= imgheight ) {
            continue;
        }

        // Choose Section
        const QVector<VisiblePlaceMark*>  currentsec = rowsection.at( y / m_labelareaheight ); 

        // Specify font properties
	// FIXME: The 1 is because the textwidth isn't saved in the
	//        mark any more.  See also the FIXME below.
        if ( 1 || textpixmap.isNull() ) {

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

            textwidth = ( QFontMetrics( font ).width( mark->name() )
                          + (int)( outlineWidth ) );
        }
        else {
            // FIXME: Find a way to know which VisiblePlaceMark that
            //        corresponds to this one (see the '1 &&' above).
#if 0
            textwidth = ( mark->textRect() ).width();
#endif
        }

	// Get a VisiblePlaceMark from the pool, or generate a new one
	// if it's empty.
	if ( m_visiblePlacemarksPool.isEmpty() ) {
	    visibleMark = new VisiblePlaceMark( mark );
	}
	else {
	    visibleMark = m_visiblePlacemarksPool.last();
	    m_visiblePlacemarksPool.pop_back();
            visibleMark->setPlaceMark( mark );
	}

        // Find out whether the area around the placemark is
        // covered already.

        bool  overlap     = true;
        int   symbolwidth = mark->symbolSize().width();

        int  xpos = symbolwidth / 2 + x + 1;
        int  ypos = 0;

        while ( xpos >= x - textwidth - symbolwidth - 1
                && overlap == true )
        {
            ypos = y; 
	
            while ( ypos >= y - m_fontheight && overlap == true) { 

                overlap = false;

                QRect  textRect( xpos, ypos, textwidth, m_fontheight );

		// Find a position for the label somewhere around the symbol.
                for ( QVector<VisiblePlaceMark*>::const_iterator beforeit = currentsec.constBegin();
                      beforeit != currentsec.constEnd();
                      ++beforeit )
                {
                    if ( textRect.intersects( (*beforeit)->labelRect()) ) {
                        overlap = true;
                        break;
                    }
                }

                if ( overlap == false ) {
                    visibleMark->setLabelRect( textRect );
                }
                ypos -= m_fontheight; 
            }

            xpos -= ( symbolwidth + textwidth + 2 );
        }

        // Calculate a position for the label if we can find an area
        // for it, and generate the pixmal for it..
        if ( !overlap ) {
            if ( textpixmap.isNull() ) {
                // Draw the text on the label.

                // Due to some XOrg bug this requires a
                // workaround via QImage in some cases.

                if ( !m_useworkaround ) {
                    textpixmap = QPixmap( textwidth, m_fontheight );
                    textpixmap.fill( Qt::transparent );

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
                    QImage textimage( textwidth, m_fontheight,
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

                visibleMark->setLabelPixmap( textpixmap );
            }

            // Finally save the label position on the map.
            visibleMark->setSymbolPos( x - mark->symbolSize().width()  / 2,
				       y - mark->symbolSize().height() / 2 );

            // Add the current placemark to the matching row and it's
            // direct neighbors.
            int idx = y / m_labelareaheight;
            if ( idx - 1 >= 0 )
                rowsection[ idx - 1 ].append( visibleMark );
            rowsection[ idx ].append( visibleMark );
            if ( idx + 1 < secnumber )
                rowsection[ idx + 1 ].append( visibleMark );

	    // Append the VisiblePlaceMark to the list and check if
	    // there is no room for any more.
	    //
	    // FIXME: Find a better way to reduce clutter.
            m_visiblePlacemarks.append( visibleMark );
            labelnum ++;
            if ( labelnum >= maxlabels )
                break;				
        }
    }

    // Finally, actually paint the placemarks that are visible and
    // their labels.
    QVector<VisiblePlaceMark*>::const_iterator  visit = m_visiblePlacemarks.constEnd();

    while ( visit != m_visiblePlacemarks.constBegin() ) {
        --visit;
        visibleMark = *visit;
        //qDebug() << "Mark: " << visibleMark->placeMark()->name();
        painter->drawPixmap( visibleMark->labelRect(),
                             visibleMark->labelPixmap() );
        painter->drawPixmap( visibleMark->symbolPos(),
                             visibleMark->symbolPixmap() );
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

#if 0
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
#else
    QVector<VisiblePlaceMark*>::const_iterator  it;

    for ( it = m_visiblePlacemarks.constBegin();
          it != m_visiblePlacemarks.constEnd();
          it++ )
    {
        VisiblePlaceMark  *mark = *it; // no cast

        if ( mark->labelRect().contains( curpos )
             || QRect( mark->symbolPos(), mark->symbolSize() ).contains( curpos ) ) {
            ret.append( mark->placeMark() );
        }
    }
#endif
    return ret;
}


#include "placemarkpainter.moc"
