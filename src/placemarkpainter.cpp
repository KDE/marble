#include "placemarkpainter.h"

#include <QAbstractItemModel>
#include <QIcon>
#include <QPainter>
#include "placecontainer.h"
#include "placemark.h"
#include "katlasdirs.h"

#include <QDebug>

PlaceMarkPainter::PlaceMarkPainter(QObject* parent) : QObject(parent) {
	m_font = QFont("Sans Serif",8,QFont::Normal);
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

	QImage image( 1000 , m_fontheight, QImage::Format_ARGB32_Premultiplied);
	image.fill(0); 
	m_empty = QPixmap::fromImage(image);

}

void PlaceMarkPainter::paintPlaceMark(QPainter* painter, int x, int y, const QAbstractItemModel* model, int row){
	QModelIndex mpic = model->index(row,0,QModelIndex());
	QIcon icon = (model->data(mpic, Qt::DecorationRole)).value<QIcon>();
	QModelIndex mnametag = model->index(row,0,QModelIndex());
	QString nametag = model->data(mnametag, Qt::DisplayRole).toString();
//	painter->translate(x-4,y-4);
	painter->drawPixmap(x-4, y-4, icon.pixmap(12,12));

//	painter->translate(8,8);
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
	painter->setFont(m_font);

	QPixmap textpixmap;
	QPainter textpainter; // begin?

	PlaceContainer::const_iterator it;

	for ( it=placecontainer->constBegin(); it != placecontainer->constEnd(); it++ ){ // STL-Iteratoren

		PlaceMark* mark  = *it; // kein Cast

		*qpos = mark->getQuatPoint();

		qpos->rotateAroundAxis(invRotAxis);

		textpixmap = mark->textPixmap();

		if ( qpos->v[Q_Z] > 0 ){

			x = (int)(imgrx + radius*qpos->v[Q_X]);
			y = (int)(imgry + radius*qpos->v[Q_Y]);

			if ( x >= 0 && x < imgwidth && y >= 0 && y < imgheight ){

				painter->drawPixmap( x-4, y-4 , m_citysymbol.at( mark->symbol() ));

				if ( textpixmap.isNull() == true ){
					int fontwidth = QFontMetrics(m_font).boundingRect(mark->name()).width();

					textpixmap = m_empty.copy( QRect( 0, 0, fontwidth+10, m_fontheight) );

					textpainter.begin( &textpixmap );
					textpainter.drawText( 0, m_fontascent, mark->name() );
					textpainter.end();

					mark->setTextPixmap( textpixmap );
				}

				painter->drawPixmap( x + 2, y, textpixmap );

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
