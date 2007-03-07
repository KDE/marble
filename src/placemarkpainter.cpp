#include "placemarkpainter.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtCore/QList>
#include <QtGui/QPainter>
#include <QtCore/QPoint>
#include <QtCore/QVectorIterator>
#include "placemark.h"
#include "katlasdirs.h"

#ifdef KDEBUILD
#include "placemarkpainter.moc"
#endif

#include <QtCore/QDebug>


static const int maxlabels = 100;

PlaceMarkPainter::PlaceMarkPainter(QObject* parent) : QObject(parent) {

	m_font_mountain = QFont("Sans Serif",7, 50, false );

	m_font_regular = QFont("Sans Serif",8, 50, false );
	m_font_regular_italics = QFont("Sans Serif",8, 50, true );
	m_font_regular_underline = QFont("Sans Serif",8, 50, false );
	m_font_regular_underline.setUnderline( true );
	
	m_fontheight = QFontMetrics(m_font_regular).height();
	m_fontascent = QFontMetrics(m_font_regular).ascent();

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

	m_useworkaround = testbug();
//	m_useworkaround = true;
}

void PlaceMarkPainter::paintPlaceFolder(QPainter* painter, int imgrx, int imgry, int radius, const PlaceContainer* placecontainer, Quaternion rotAxis ){

	int imgwidth = 2 * imgrx; int imgheight = 2 * imgry;
	int x = 0; int y = 0; 

	int secnumber = imgheight / m_labelareaheight + 1;
/*
				if ( mark->name().contains( "London" ) ){
					qDebug() << "London" << " y: " << QString::number( y ) << " qpos.v[Q_Y]: " << QString::number( qpos.v[Q_Y] );
					invrotAxis.display();
				}
*/
//	rotAxis.display();

	Quaternion invRotAxis = rotAxis.inverse();
	Quaternion qpos;

	painter->setPen(QColor(Qt::black));	

	QPainter textpainter;

	QFont font;
	int fontwidth = 0;

	QPixmap textpixmap;

	QVector < QVector < PlaceMark* > > m_rowsection;
	for ( int i = 0; i < secnumber; i++) m_rowsection.append( QVector<PlaceMark*>( ) );

//	QPen outlinepen( QColor( 255,255,255,160 ) );
//	outlinepen.setWidth( 1 );
//	QBrush outlinebrush( QColor( 255,255,255,160 ) );

//	QPainterPathStroker stroker;
//	stroker.setWidth( 1 );

//	QBrush shapebrush( QColor( 0,0,0,255) );
//	const QPointF baseline( 0.0f , (float)(m_fontascent) );

	m_visibleplacemarks.clear();

	PlaceContainer::const_iterator it;
	PlaceMark* mark  = 0; 

	int labelnum = 0;

	for ( it=placecontainer->constBegin(); it != placecontainer->constEnd(); it++ ){ // STL iterators


		mark  = *it; // no cast

		if ( m_weightfilter.at(mark->popidx()) > radius && mark->symbol() != 0 && mark-> selected() == 0 ) continue; 

		qpos = mark->getQuatPoint();

		qpos.rotateAroundAxis(invRotAxis);

		textpixmap = mark->textPixmap();

		if ( qpos.v[Q_Z] > 0 ){

			x = (int)(imgrx + radius*qpos.v[Q_X]);
			y = (int)(imgry + radius*qpos.v[Q_Y]);

			// Don't process placemarks if they are outside the screen area
			if ( x >= 0 && x < imgwidth && y >= 0 && y < imgheight ){

				// Choose Section
				const QVector<PlaceMark*> currentsec = m_rowsection.at( y / m_labelareaheight ); 

				// Specify font properties
		
				if ( textpixmap.isNull() == true ){	

					QChar role = mark->role();

					if ( role == 'N' ){ 
						font = m_font_regular;
					} else if ( role == 'R' ){ 
						font = m_font_regular_italics;
					} else if ( role == 'B' || mark->role() == 'C' ) {
						font = m_font_regular_underline;
					} else {
						font = m_font_regular;
					}

					if ( mark->symbol() > 13 ) font.setWeight( 75 );

					if ( role == 'P' ) 
						font = m_font_regular;
					if ( role == 'M' ) 
						font = m_font_regular;
					if ( role == 'H' ) 
						font = m_font_mountain;
					if ( role == 'V' ) 
						font = m_font_mountain;

					fontwidth = QFontMetrics(font).width(mark->name());
				}
				else{
					fontwidth = ( mark->textRect() ).width();
				}

				// Find out whether the area around the placemark is covered already

				bool overlap = true;
				int symbolwidth = mark->symbolSize().width();

				int xpos = symbolwidth/2 + x + 1;
				int ypos = 0;

				while ( xpos >= x - fontwidth - symbolwidth - 1 && overlap == true ) { 

					ypos = y; 
	
					while ( ypos >= y - m_fontheight && overlap == true) { 

						overlap = false;

						QRect textRect( xpos, ypos, fontwidth, m_fontheight ); 


						for ( QVector<PlaceMark*>::const_iterator beforeit = currentsec.constBegin(); beforeit != currentsec.constEnd(); beforeit++ ){ // STL iterators
							if ( textRect.intersects( (*beforeit) -> textRect()) ){
								overlap = true;
								break;
							}
						}
					
						if ( overlap == false ){
							mark->setTextRect( textRect );				
						}
						ypos -= m_fontheight; 
					}

					xpos -= ( symbolwidth + fontwidth + 2 );
				}

				// Paint the label
				if ( overlap == false) {
					if ( textpixmap.isNull() == true ){					
						// Draw the text on the label

						// Due to some XOrg bug this requires a
						// workaround via QImage in some cases
/*
						QPainterPath shapepath;
						shapepath.addText( baseline, m_font, mark->name() );
						QPainterPath outlinepath = stroker.createStroke(shapepath);
*/
						if ( !m_useworkaround ) {
							textpixmap = QPixmap( fontwidth, m_fontheight );
							textpixmap.fill(Qt::transparent);

							textpainter.begin( &textpixmap );

							textpainter.setFont(font);
							textpainter.setPen(m_labelcolor);	
							textpainter.drawText( 0, m_fontascent, mark->name() );

//							textpainter.setRenderHint(QPainter::Antialiasing, true);
//							textpainter.fillPath( outlinepath, outlinebrush );
//							textpainter.fillPath( shapepath, shapebrush );

							textpainter.end();
						}
						else {
							QImage textimage( fontwidth, m_fontheight, QImage::Format_ARGB32_Premultiplied );
							textimage.fill( 0 );

							textpainter.begin( &textimage );

							textpainter.setFont(font);
							textpainter.setPen(m_labelcolor);	
							textpainter.drawText( 0, m_fontascent, mark->name() );

							textpainter.end();
							
							textpixmap = QPixmap::fromImage( textimage );
						}


						mark->setTextPixmap( textpixmap );
					}
					// Finally save the label position on the map

					mark->setSymbolPos( QPoint( x - mark->symbolSize().width()/2, y - mark->symbolSize().height()/2) );

					// Add the current placemark to the matching row and it's
					// direct neighbors
					int idx = y / m_labelareaheight;
					if ( idx - 1 >= 0 )  m_rowsection[ idx - 1 ].append( mark );
					m_rowsection[ idx ].append( mark );
					if ( idx + 1 < secnumber )  m_rowsection[ idx + 1 ].append( mark );

					m_visibleplacemarks.append(mark);
					labelnum ++;
					if ( labelnum >= maxlabels ) break;				
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
//	qDebug() << QString("Size: %1, Rows: %2").arg(m_visibleplacemarks.size()).arg( secnumber );
	PlaceContainer::const_iterator visit = m_visibleplacemarks.constEnd();

	while (visit != m_visibleplacemarks.constBegin()) {
		--visit;
		mark = *visit;
		painter->drawPixmap( mark -> textRect(), mark -> textPixmap() );
		painter->drawPixmap( mark -> symbolPos(), mark -> symbolPixmap()  );
	}
}

bool PlaceMarkPainter::testbug(){

	QString testchar("K");
	QFont font("Sans Serif",10);

	int fontheight = QFontMetrics(font).height();
	int fontwidth = QFontMetrics(font).width(testchar);
	int fontascent = QFontMetrics(font).ascent();

	QPixmap pixmap ( fontwidth, fontheight );
	pixmap.fill(Qt::transparent);

	QPainter textpainter;
	textpainter.begin(&pixmap);
		textpainter.setPen( QColor(0,0,0,255) );
		textpainter.setFont( font );
		textpainter.drawText( 0, fontascent, testchar );
	textpainter.end();

	QImage image = pixmap.toImage();

	for (int x = 0; x < fontwidth; x++)
		for (int y = 0; y < fontheight; y++){
			if ( qAlpha( image.pixel( x,y ) ) > 0 ) return false;
		}

	return true;
}

QVector<PlaceMark*> PlaceMarkPainter::whichPlaceMarkAt( const QPoint& curpos ){

	QVector<PlaceMark*> ret;

	PlaceContainer::const_iterator it;

	for ( it = m_visibleplacemarks.constBegin(); it != m_visibleplacemarks.constEnd(); it++ ){ // STL iterators
		PlaceMark* mark  = *it; // no cast

		if ( mark->textRect().contains( curpos ) || QRect( mark->symbolPos(), mark->symbolSize() ).contains( curpos ) ){
			ret.append( mark );
		}
	}

	return ret;
}
