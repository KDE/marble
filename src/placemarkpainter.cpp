#include "placemarkpainter.h"

#include <QAbstractItemModel>
#include <QDebug>
#include <QIcon>
#include <QList>
#include <QPainter>
#include <QPoint>
#include <QVectorIterator>
#include <QX11Info>
#include "placecontainer.h"
#include "placemark.h"
#include "katlasdirs.h"

#ifdef KDEBUILD
#include "placemarkpainter.moc"
#endif

#include <QDebug>


PlaceMarkPainter::PlaceMarkPainter(QObject* parent) : QObject(parent) {

	m_font_regular = QFont("Sans Serif",8, 50, false );
	m_font_regular_italics = QFont("Sans Serif",8, 50, true );
	m_font_regular_underline = QFont("Sans Serif",8, 50, false );
	m_font_regular_underline.setUnderline( true );
	
	m_fontheight = QFontMetrics(m_font_regular).height();
	m_fontascent = QFontMetrics(m_font_regular).ascent();

	m_labelcolor = QColor( 0, 0, 0, 255 );

	m_citysymbol 
	 << QPixmap(KAtlasDirs::path("bitmaps/city_4_white.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_4_yellow.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_4_orange.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_4_red.png"))

	 << QPixmap(KAtlasDirs::path("bitmaps/city_3_white.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_3_yellow.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_3_orange.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_3_red.png"))

	 << QPixmap(KAtlasDirs::path("bitmaps/city_2_white.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_2_yellow.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_2_orange.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_2_red.png"))

	 << QPixmap(KAtlasDirs::path("bitmaps/city_1_white.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_1_yellow.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_1_orange.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_1_red.png"));

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
}

void PlaceMarkPainter::paintPlaceMark(QPainter* painter, int x, int y, const QAbstractItemModel* model, int row){

	QModelIndex mpic = model->index(row,0,QModelIndex());
	QIcon icon = (model->data(mpic, Qt::DecorationRole)).value<QIcon>();
	QModelIndex mnametag = model->index(row,0,QModelIndex());
	QString nametag = model->data(mnametag, Qt::DisplayRole).toString();

	painter->drawPixmap(x-4, y-4, icon.pixmap(12,12));

	painter->setPen(QColor(Qt::black));	
	painter->setFont(m_font_regular);
	painter->drawText(x+8, y+8, nametag);

}

void PlaceMarkPainter::paintPlaceFolder(QPainter* painter, int imgrx, int imgry, int radius, const PlaceContainer* placecontainer, Quaternion rotAxis ){

	int imgwidth = 2 * imgrx; int imgheight = 2 * imgry;
	int x = 0; int y = 0; 

	Quaternion invRotAxis = rotAxis.inverse();
	Quaternion* qpos = new Quaternion();

	painter->setPen(QColor(Qt::black));	

	QPainter textpainter;

	QFont font;
	int fontwidth = 0;

	QPixmap textpixmap;


//	QPen outlinepen( QColor( 255,255,255,160 ) );
//	outlinepen.setWidth( 1 );
//	QBrush outlinebrush( QColor( 255,255,255,160 ) );

//	QPainterPathStroker stroker;
//	stroker.setWidth( 1 );

//	QBrush shapebrush( QColor( 0,0,0,255) );


//	const QPointF baseline( 0.0f , (float)(m_fontascent) );

//	qDebug() << QString("Radius: %1").arg(radius); 

	QVector<PlaceMark*> visibleplacemarks;
	visibleplacemarks.clear();

	PlaceContainer::const_iterator it;

	for ( it=placecontainer->constBegin(); it != placecontainer->constEnd(); it++ ){ // STL iterators


		PlaceMark* mark  = *it; // no cast

		if ( m_weightfilter[mark->symbol()] > radius && mark->symbol() != 0 ) continue; 

		*qpos = mark->getQuatPoint();

		qpos->rotateAroundAxis(invRotAxis);

		textpixmap = mark->textPixmap();

		if ( qpos->v[Q_Z] > 0 ){

			x = (int)(imgrx + radius*qpos->v[Q_X]);
			y = (int)(imgry + radius*qpos->v[Q_Y]);

			// Don't process placemarks if they are outside the screen area
			if ( x >= 0 && x < imgwidth && y >= 0 && y < imgheight ){

				// Specify font properties
		
				if ( textpixmap.isNull() == true ){	

					if ( mark->role() == 'N' ){ 
						font = m_font_regular;
					} else if ( mark->role() == 'R' ){ 
						font = m_font_regular_italics;
					} else if ( mark->role() == 'B' || mark->role() == 'C' ) {
						font = m_font_regular_underline;
					} else {
						font = m_font_regular;
					}

					if ( mark->symbol() > 13 ) font.setWeight( 75 );

					fontwidth = QFontMetrics(font).width(mark->name());
				}
				else{
					fontwidth = ( mark->textRect() ).width();
				}

				// Find out whether the area around the placemark is covered already

				bool overlap = true;
				int xpos = x + 2;

				while ( xpos >= x - fontwidth - 2 && overlap == true ) { 

					int ypos = y;

					while ( ypos >= y - m_fontheight && overlap == true) { 

						overlap = false;

						QRect textRect( xpos, ypos, fontwidth, m_fontheight ); 

						for ( QVector<PlaceMark*>::const_iterator beforeit = visibleplacemarks.constBegin(); beforeit != visibleplacemarks.constEnd(); beforeit++ ){ // STL iterators
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

					xpos -= ( fontwidth + 4 );
				}

				// Paint the label
				if ( overlap == false) {
					if ( textpixmap.isNull() == true ){					
						// Draw the text on the label
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
					// Paint the label onto the map

					mark->setSymbolPos( QPoint( x-4, y-4 ) );
					visibleplacemarks.append(mark);					
				}
				else {
					if ( mark->symbol() == 0 )
						mark->setSymbolPos( QPoint( x-4, y-4 ) );
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

	QVector<PlaceMark*>::const_iterator visit = visibleplacemarks.constEnd();
	while (visit != visibleplacemarks.constBegin()) {
		--visit;
		painter->drawPixmap( (*visit) -> textRect(), (*visit) -> textPixmap() );
		painter->drawPixmap( (*visit) -> symbolPos(), m_citysymbol[ (*visit) -> symbol() ] );
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
