#include "katlaswindrose.h"

#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QPainterPath>
#include <QtSvg/QSvgRenderer>
#include "katlasdirs.h"

#include <QtCore/QDebug>

#ifdef KDEBUILD
#include "katlaswindrose.moc"
#endif

KAtlasWindRose::KAtlasWindRose(QObject* parent) : QObject(parent) {
	m_width = 0;

	m_font = QFont("Sans Serif",8 );
	m_font.setStyleStrategy(QFont::ForceOutline);

	m_fontheight = QFontMetrics(m_font).ascent();
	m_fontwidth = QFontMetrics(m_font).boundingRect("N").width();
	m_svgobj = new QSvgRenderer( KAtlasDirs::path("svg/windrose.svg"), this );

	m_polarity = 1;
}

QPixmap& KAtlasWindRose::drawWindRosePixmap(int canvaswidth, int canvasheight, int northpoley ){

	int width = 52;
	int polarity = 0;

	if ( northpoley != 0 ) 
		polarity = northpoley / abs(northpoley);
	else
		polarity = 0;		

	if (width == m_width && polarity == m_polarity)
		return m_pixmap;
	m_width = width; m_polarity = polarity;

	m_pixmap = QPixmap( m_width, m_width + m_fontheight + 5 );
	m_pixmap.fill(Qt::transparent);

	QPainter painter(&m_pixmap);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen( Qt::black );
	painter.setBrush(QColor(192,192,192,192));

	QString dirstr = tr("N");
	if ( polarity == 1 ) dirstr = tr("S");
	if ( polarity == 0 ) dirstr = tr("");

	m_fontwidth = QFontMetrics(m_font).boundingRect( dirstr ).width();

	QRect viewport( 1, m_fontheight + 5 + 1, m_width - 1, m_width - 1 ); 
	painter.drawEllipse( QRectF(viewport) );

	QPen outlinepen( QColor( 192,192,192,192 ) );
	outlinepen.setWidth( 2 );
	QBrush outlinebrush( Qt::black );

	QPainterPath outlinepath;
	const QPointF baseline( 0.5f * (float)( m_width - m_fontwidth ) , (float)(m_fontheight) + 2.0f );
	outlinepath.addText( baseline, m_font, dirstr);

	painter.setPen(outlinepen);
	painter.setBrush(outlinebrush);
	painter.drawPath( outlinepath );
	painter.setPen( Qt::NoPen );
	painter.drawPath( outlinepath );

	painter.setViewport( viewport );
	m_svgobj->render(&painter); 


	return m_pixmap;

}
