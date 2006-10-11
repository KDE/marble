#include "placemarkpainter.h"

#include <QAbstractItemModel>
#include <QDebug>
#include <QIcon>
#include <QList>
#include <QPainter>
#include <QPoint>
#include <QX11Info>
#include "placecontainer.h"
#include "placemark.h"
#include "katlasdirs.h"

#ifdef KDEBUILD
#include "placemarkpainter.moc"
#endif

#include <QDebug>


PlaceMarkPainter::PlaceMarkPainter(QObject* parent) : QObject(parent) {

	m_font = QFont("Sans Serif",8);
	m_labelcolor = QColor( 0, 0, 0, 255 );

	m_font.setStyleStrategy(QFont::ForceOutline);
	m_fontheight = QFontMetrics(m_font).height();
	m_fontascent = QFontMetrics(m_font).ascent();
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
}

void PlaceMarkPainter::paintPlaceMark(QPainter* painter, int x, int y, const QAbstractItemModel* model, int row){

	QModelIndex mpic = model->index(row,0,QModelIndex());
	QIcon icon = (model->data(mpic, Qt::DecorationRole)).value<QIcon>();
	QModelIndex mnametag = model->index(row,0,QModelIndex());
	QString nametag = model->data(mnametag, Qt::DisplayRole).toString();

	painter->drawPixmap(x-4, y-4, icon.pixmap(12,12));

	painter->setPen(QColor(Qt::black));	
	painter->setFont(m_font);
	painter->drawText(x+8, y+8, nametag);

}

void PlaceMarkPainter::paintPlaceFolder(QPainter* painter, int imgrx, int imgry, int radius, const PlaceContainer* placecontainer, Quaternion rotAxis ){

	int imgwidth = 2 * imgrx; int imgheight = 2 * imgry;
	int x = 0; int y = 0; 

	Quaternion invRotAxis = rotAxis.inverse();
	Quaternion* qpos = new Quaternion();

	painter->setPen(QColor(Qt::black));	

	QPainter textpainter;
	

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

				// Draw placemark symbol
				m_font.setWeight( 50 );

				if ( mark->role() == 'R' ){ 
					m_font.setItalic( true );
//					m_font.setWeight( 63 );
				}
				else {
					m_font.setItalic( false );
				}
				if ( mark->role() == 'B' || mark->role() == 'C' ) 
					m_font.setUnderline( true );
				else
					m_font.setUnderline( false );
				if ( mark->symbol() > 13 )
					m_font.setWeight( 75 );
		
				int fontwidth = QFontMetrics(m_font).width(mark->name());

				bool overlap = false;

				const QSize textSize( fontwidth, m_fontheight );

				// Possible label orientations around the symbol
				QPoint bottomRight( x + 2, y );
				QPoint bottomLeft( x - fontwidth - 2, y );
				QPoint topRight( x + 2, y - m_fontheight );
				QPoint topLeft( x - fontwidth - 2, y - m_fontheight );
				

				QPoint labelplace = bottomRight;
				
				// Find out whether the area around the placemark is covered already

				foreach ( labelplace, QList<QPoint>() << bottomRight << topRight << bottomLeft <<  topLeft ) { 

					overlap = false;

					const QRect textRect = QRect( labelplace, textSize ); 

					for ( QVector<PlaceMark*>::const_iterator beforeit = visibleplacemarks.constBegin(); beforeit != visibleplacemarks.constEnd(); beforeit++ ){ // STL iterators
						if ( textRect.intersects( (*beforeit) -> textRect()) ){
							overlap = true;
							break;
						}
					}
					
					if ( overlap == false ){
						mark->setTextRect( textRect );				
						break;
					}
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
						textpixmap = QPixmap( fontwidth, m_fontheight );
						textpixmap.fill(Qt::transparent);

						textpainter.begin( &textpixmap );

						textpainter.setFont(m_font);

//						m_labelcolor.setAlpha(255);
						textpainter.setPen(m_labelcolor);	

						textpainter.drawText( 0, m_fontascent, mark->name() );
/*
						textpainter.setRenderHint(QPainter::Antialiasing, true);
						textpainter.fillPath( outlinepath, outlinebrush );
						textpainter.fillPath( shapepath, shapebrush );
*/
						textpainter.end();

						mark->setTextPixmap( textpixmap );
					}
					// Paint the label onto the map
					painter->drawPixmap( labelplace, textpixmap );
					visibleplacemarks.append(mark);
					painter->drawPixmap( x-4, y-4 , m_citysymbol[mark->symbol()]);
				}
				else {
					if ( mark->symbol() == 0 )
						painter->drawPixmap( x-4, y-4 , m_citysymbol[mark->symbol()]);
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
}
