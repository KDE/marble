#include "katlasflag.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPixmapCache>
#include <QSvgRenderer>
#include "katlasdirs.h"

#include <QDebug>

#ifdef KDEBUILD
#include "katlaswindrose.moc"
#endif

KAtlasFlag::KAtlasFlag( QObject* parent ) : QObject(parent) {
	QPixmapCache::setCacheLimit ( 384 );
}

void KAtlasFlag::setFlag( QString filename, QSize size ){
	m_filename = filename;
	m_size = size;
}

void KAtlasFlag::slotDrawFlag(){

	QString keystring = QString( m_filename ).replace( "flags/", "" );
	if (!QPixmapCache::find( keystring, m_pixmap ) ) {
		m_pixmap = QPixmap( m_size );
		m_pixmap.fill(Qt::transparent);
		QSvgRenderer svgobj( m_filename, this );
		QPainter painter( &m_pixmap );
		painter.setRenderHint(QPainter::Antialiasing, true);
		QRect viewport( m_pixmap.rect() );
		painter.setViewport( viewport );
		svgobj.render(&painter);
     		QPixmapCache::insert(keystring, m_pixmap);
	}
	emit flagDone();
}
