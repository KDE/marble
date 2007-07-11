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
// #define FLAT_PROJ
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


#if 1

void PlaceMarkPainter::paintPlaceFolder(QPainter* painter, 
                                        int imgwidth, int imgheight,
                                        int radius,
                                        const PlaceMarkContainer* placecontainer,
                                        Quaternion planetAxis )
{
    //int  imgwidth  = 2 * imgrx;
    //int  imgheight = 2 * imgry;
    int  x = 0;
    int  y = 0; 

    int  secnumber = imgheight / m_labelareaheight + 1;
#if 0
      if ( mark->name().contains( "London" ) ){
      qDebug() << "London" << " y: " << QString::number( y ) << " qpos.v[Q_Y]: " << QString::number( qpos.v[Q_Y] );
      invplanetAxis.display();
      }
#endif
    // planetAxis.display();

    Quaternion  invplanetAxis = planetAxis.inverse();
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

    m_visiblePlacemarks.clear();

    PlaceMark  *mark = 0; 
    int         labelnum = 0;
#ifdef FLAT_PROJ
    float const centerLat=planetAxis.pitch();
    float const centerLng=-planetAxis.yaw();
#endif
    PlaceMarkContainer::const_iterator  it;
    for ( it = placecontainer->constBegin();
          it != placecontainer->constEnd();
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
             && ( mark->symbol() >= 16 && mark->symbol() <= 20 ) )
            continue;

        // Skip city marks if we're not showing cities.
        if ( m_showCities == false
             && ( mark->symbol() >= 0 && mark->symbol() < 16 ) )
            continue;

        qpos = mark->quaternion();
        textpixmap = mark->textPixmap();

#ifndef FLAT_PROJ
        qpos.rotateAroundAxis(invplanetAxis);

        if ( qpos.v[Q_Z] > 0 ) {

            x = (int)(imgwidth/2 + radius * qpos.v[Q_X]);
            y = (int)(imgheight/2 + radius * qpos.v[Q_Y]);

#else
        if( true )
        {
            double xyFactor = (float)(2*radius)/M_PI;

            double degX;
            double degY;
            qpos.getSpherical(degX,degY);

            x = (int)(imgwidth/2 + xyFactor * (degX + centerLng));
            y = (int)(imgheight/2 + xyFactor * (degY + centerLat));
#endif

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

                    m_visiblePlacemarks.append(mark);
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

    // qDebug() << QString("Size: %1, Rows: %2").arg(m_visiblePlacemarks.size()).arg( secnumber );
    PlaceMarkContainer::const_iterator  visit = m_visiblePlacemarks.constEnd();

    while ( visit != m_visiblePlacemarks.constBegin() ) {
        --visit;
        mark = *visit;
        int tempSymbol = mark->symbolPos().x();
        int tempText = mark->textRect().left();

        painter->drawPixmap( mark->textRect(),  mark->textPixmap() );
        painter->drawPixmap( mark->symbolPos(), mark->symbolPixmap() );
#ifdef FLAT_PROJ
        for(int i = tempSymbol - 4*radius; i>=0 ; i-= 4*radius) {
            mark->textRect().moveLeft(i - tempSymbol + tempText );
            mark->symbolPos().setX( mark->symbolPos().x() - 4*radius);
            painter->drawPixmap( mark->textRect(),  mark->textPixmap() );
            painter->drawPixmap( mark->symbolPos(), mark->symbolPixmap() );
        }

        mark->symbolPos().setX(tempSymbol);

        for(int i = tempSymbol + 4*radius; i<=imgwidth ; i+= 4*radius) {
            mark->textRect().moveLeft(i - tempSymbol + tempText );
            mark->symbolPos().setX( mark->symbolPos().x() + 4*radius);
            painter->drawPixmap( mark->textRect(),  mark->textPixmap() );
            painter->drawPixmap( mark->symbolPos(), mark->symbolPixmap() );
        }
        mark->textRect().moveLeft(tempSymbol);
        mark->symbolPos().setX(tempText);
#endif
    }
}


#else

void PlaceMarkPainter::paintPlaceFolder(QPainter* painter, 
                                        int imgwidth,
                                        int imgheight,
                                        int radius,
                                        const PlaceMarkContainer* placeMarkContainer,
                                        Quaternion planetAxis )
{
    int  x = 0;
    int  y = 0; 

    int  secnumber = imgheight / m_labelareaheight + 1;
#if 0
      if ( mark->name().contains( "London" ) ){
      qDebug() << "London" << " y: " << QString::number( y ) << " qpos.v[Q_Y]: " << QString::number( qpos.v[Q_Y] );
      invplanetAxis.display();
      }
#endif
    // planetAxis.display();

    painter->setPen(QColor(Qt::black));	

    QPainter    textpainter;
    QFont       font;
    QPixmap     textpixmap;

    const double outlineWidth = 2.5;
    int          textwidth = 0;

    // This is for the layout of placemark labels.
    //
    // Introduce a QVector of QVectors.  Each QVector contains a
    // pointer to the VisiblePlaceMarks that have their names visible
    // on that row.  A "row" is not a pixel row, but a row of names
    // that are overlaid the picture.  Each row is one fontHeight()
    // pixel high.  This is later used for collision detection for the
    // labels.
    QVector< QVector< VisiblePlaceMark* > >  rowsection;
    for ( int i = 0; i < secnumber; i++)
        rowsection.append( QVector<VisiblePlaceMark*>( ) );

    // Check for the visible PlaceMarks from last paint operation that
    // are still visible and remove those that aren't.  The
    // VisiblePlaceMarks that belong to PlaceMarks that were visible
    // but aren't any more, are collected into a pool for later reuse.
    QList<VisiblePlaceMark*>::iterator  it = m_visiblePlacemarks.begin();
    while ( it != m_visiblePlacemarks.constEnd() ) {
	if ( 0 && isVisible( (*it)->placeMark(), radius, planetAxis, 
			imgwidth, imgheight, 
			x, y ) )
	    ++it;
	else {
	    m_visiblePlacemarksPool.append( *it );
	    it = m_visiblePlacemarks.erase( it );
	}
    }

    PlaceMark         *mark = 0; 
    VisiblePlaceMark  *visibleMark = 0; 
#if 0
    int                numLabels = m_visiblePlacemarks.size();
#else
    int                numLabels = 0;
#endif


    //qDebug() << "-----------------------------------------------------------";
    //qDebug() << "numLabels = " << numLabels;

    // Loop through ALL PlaceMarks and collect those that are visible.
    // All the visible ones are put into m_visiblePlacemarks, a
    // QVector of VisiblePlaceMarks.
    PlaceMarkContainer::const_iterator  it2;
    for ( it2 = placeMarkContainer->constBegin();
          it2 != placeMarkContainer->constEnd();
          ++it2 ) 
    {
        mark  = *it2; // no cast

	// If the PlaceMark is not visible, go to next PlaceMark.
        if ( !isVisible( mark, radius, planetAxis, imgwidth, imgheight,
                         x, y ) ) {
	    //qDebug() << mark->name() << ": Not visible";
            continue;
	}

	// Check if the PlaceMark already is marked as visible.
	bool  found;

	found = false;
	it = m_visiblePlacemarks.begin();
	while ( it != m_visiblePlacemarks.constEnd() ) {
	    if ( (*it)->placeMark() == mark ) {
		found = true;

		visibleMark = *it;
		textpixmap = (*it)->labelPixmap();

		break;
	    }

	    ++it;
	}

	// Get a new VisiblePlaceMark if we didn't find a vious
	// one. Either get it from the pool, or generate a new one.
	if ( !found ) {
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

	    textpixmap = QPixmap();
	}


        // Ok, the placemark is visible. Now take care of fixing a
        // label if necessary.

        // Choose Section
	//qDebug() << mark->name() << ": y=" << y << (found ? " (found) " : "");
        const QVector<VisiblePlaceMark*>  currentsec = rowsection.at( y / m_labelareaheight ); 

        // Specify font properties, especially get the textwidth.
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

            textwidth = ( QFontMetrics( font ).width( mark->name() )
                          + (int)( outlineWidth ) );
        }
        else {
            textwidth = ( visibleMark->labelRect() ).width();
        }

        // Find out whether the area around the placemark is
        // covered already. It also 
        bool  overlap = !roomForLabel( currentsec,
                                       visibleMark, mark,
                                       textwidth, x, y );

        // Calculate a position for the label if we can find an area
        // for it, and generate the pixmap for it.
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
                                      QImage::Format_ARGB32_multiplied );
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

            // Finally save the symbol position on the map.
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
	    if (!found) {
		m_visiblePlacemarks.append( visibleMark );
		numLabels ++;
	    }
#if 0
            if ( numLabels >= maxlabels )
                break;				
#endif
        }
#if 0
 else {
            it = m_visiblePlacemarks.begin();
            while ( it != m_visiblePlacemarks.constEnd() ) {
                if ( (*it)->placeMark() == mark ) {
                    m_visiblePlacemarksPool.append( *it );
                    it = m_visiblePlacemarks.erase( it );
                } else {
                    it++;
                }
               
            }
        }
#endif
    }

    // Finally, actually paint the placemarks that are visible and
    // their labels.
    QList<VisiblePlaceMark*>::const_iterator  visit = m_visiblePlacemarks.constEnd();

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

#endif


bool PlaceMarkPainter::isVisible( PlaceMark *mark, int radius,
                                  Quaternion &planetAxis,
                                  int imgwidth, int imgheight,
                                  int &x, int &y )
{
    // Skip the places that are too small and not selected.
    if ( m_weightfilter.at( mark->popidx() ) > radius
         && mark->selected() == 0 )
        return false;

    // Skip terrain marks if we're not showing terrain.
    if ( !m_showTerrain && ( 16 <= mark->symbol() && mark->symbol() <= 19 ) )
        return false;

    // Skip city marks if we're not showing cities.
    if ( !m_showCities && ( 0 <= mark->symbol() && mark->symbol() < 16 ) )
        return false;

    Quaternion  qpos       = mark->quaternion();
    Quaternion  invplanetAxis = planetAxis.inverse();
    qpos.rotateAroundAxis( invplanetAxis );

    // Skip the place if it's on the other side of the globe.
    if ( qpos.v[Q_Z] < 0 )
        return false;

    // Don't process placemarks if they are outside the screen area
    x = (int)(imgwidth / 2  + radius * qpos.v[Q_X]);
    y = (int)(imgheight / 2 + radius * qpos.v[Q_Y]);
    if ( x < 0 || x >= imgwidth || y < 0 || y >= imgheight ) {
        return false;
    }

    return true;
}


bool PlaceMarkPainter::roomForLabel( const QVector<VisiblePlaceMark*> &currentsec,
                                     VisiblePlaceMark *visibleMark,
                                     PlaceMark *mark,
                                     int textwidth,
                                     int x, int y )
{
    bool  isRoom     = false;
    int   symbolwidth = mark->symbolSize().width();
    
    int  xpos = symbolwidth / 2 + x + 1;
    int  ypos = 0;
    while ( xpos >= x - textwidth - symbolwidth - 1 && !isRoom ) {
        ypos = y; 
	
        while ( ypos >= y - m_fontheight && !isRoom) { 

            isRoom = true;

            QRect  textRect( xpos, ypos, textwidth, m_fontheight );

            // Check if there is another label or symbol that overlaps.
            for ( QVector<VisiblePlaceMark*>::const_iterator beforeit = currentsec.constBegin();
                  beforeit != currentsec.constEnd();
                  ++beforeit )
            {
                if ( textRect.intersects( (*beforeit)->labelRect()) ) {
                    isRoom = false;
                    break;
                }
            }

            if ( isRoom ) {
                // FIXME: Should this really be here?
                visibleMark->setLabelRect( textRect );
                return true;
            }
            ypos -= m_fontheight; 
        }

        xpos -= ( symbolwidth + textwidth + 2 );
    }

    return isRoom;
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
    QList<VisiblePlaceMark*>::const_iterator  it;
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
#else
    QVector<PlaceMark*>::const_iterator  it;
    for ( it = m_visiblePlacemarks.constBegin();
          it != m_visiblePlacemarks.constEnd();
          it++ )
    {
        PlaceMark  *mark = *it; // no cast

        if ( mark->textRect().contains( curpos )
             || QRect( mark->symbolPos(), mark->symbolSize() ).contains( curpos ) ) {
            ret.append( mark );
        }
    }
#endif
    return ret;
}


#include "placemarkpainter.moc"
