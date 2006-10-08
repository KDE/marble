#include <QPainter>
#include <QColor>
#include <QDebug>
#include "katlasmapscale.h"

#ifdef KDEBUILD
#include "katlasmapscale.moc"
#endif

const double earthdiameter = 6378.0;

KAtlasMapScale::KAtlasMapScale(QObject* parent) : QObject(parent) {
	m_unit = "km";
	m_scalebarwidth=250;
	m_scalebarheight=5;

	m_font = QFont("Sans Serif",8);
	m_fontheight = QFontMetrics(m_font).ascent();
	m_leftmargin = QFontMetrics(m_font).boundingRect("0").width()/2;
	m_rightmargin = QFontMetrics(m_font).boundingRect("0000").width()/2;
//	QFontMetrics fontmetrics = QFontMetrics(m_font);
//	m_fontheight

	m_invscale = m_radius = 0;
	m_scalebarkm = 0.0;
	m_bestdivisor = m_pixelinterval = m_valueinterval = 0;
}

QPixmap& KAtlasMapScale::drawScaleBarPixmap(int radius, int width){
	if ((radius == m_radius) && (m_scalebarwidth == width - m_leftmargin - m_rightmargin))
		return m_pixmap;
	m_radius = radius;

	m_pixmap = QPixmap(width + 20, m_fontheight + m_scalebarheight + 20);
	m_pixmap.fill(Qt::transparent);

	QPainter painter(&m_pixmap);
	
	painter.setPen(QColor(Qt::black));
	painter.setBrush(QColor(192,192,192,192));

	painter.translate(.5, .5);
 	painter.drawRect(0,0, m_pixmap.width()-1, m_pixmap.height()-1);
	painter.translate(4.5, 4.5);

	paintScaleBar(&painter, radius, width);	
	return m_pixmap;
}

void KAtlasMapScale::paintScaleBar(QPainter* painter, int radius, int width){

	if ((radius == m_radius) && (width == m_scalebarwidth)) return;
	m_radius = radius;

	m_scalebarwidth = width - m_leftmargin - m_rightmargin;

	m_scalebarkm = (double)(m_scalebarwidth) * earthdiameter / (double)(radius);

	calcScaleBar();

// painter->setRenderHint(QPainter::TextAntialiasing, false);
	painter->setFont(m_font);

	painter->setPen(QColor(Qt::darkGray));
	painter->setBrush(QColor(Qt::darkGray));
	painter->drawRect(m_leftmargin, m_fontheight+3, m_leftmargin + m_scalebarwidth + m_rightmargin,m_scalebarheight);

	painter->setPen(QColor(Qt::black));
	painter->setBrush(QColor(Qt::white));
	painter->drawRect(m_leftmargin,m_fontheight+3,m_bestdivisor*m_pixelinterval,m_scalebarheight);

	painter->setBrush(QColor(Qt::black));

	QString intervalstr;

	for (int j = 0; j <= m_bestdivisor; j += 2){
		if (j < m_bestdivisor)
			painter->drawRect(m_leftmargin+j*m_pixelinterval,m_fontheight+3,m_pixelinterval-1,m_scalebarheight);

		intervalstr.setNum(j*m_valueinterval);

		if (j == 0) 
			painter->drawText(0, m_fontheight, "0 " + m_unit);
		else
			painter->drawText(m_leftmargin + j*m_pixelinterval-QFontMetrics(m_font).width(intervalstr)/2,m_fontheight,intervalstr);

	}

//	return m_pic;
}

void KAtlasMapScale::calcScaleBar(){

	double magnitude = 1;
	int magvalue = (int)( m_scalebarkm );

	while (magvalue >= 100) { magvalue /= 10; magnitude*=10; }

	m_bestdivisor = 4;
	int bestmagvalue = 1;

	for (int i = 0; i < magvalue; i++){
		for (int j = 4; j < 9; j++){
			if ((magvalue-i)%j == 0){
				m_bestdivisor = j;
				bestmagvalue = magvalue - i;
				i = magvalue; 
				j = 9;
			}
		}
	}

	m_pixelinterval = (int)(m_scalebarwidth*(double)(bestmagvalue)/(double)(magvalue)/m_bestdivisor);
	m_valueinterval = (int)(bestmagvalue*magnitude/m_bestdivisor);
}
