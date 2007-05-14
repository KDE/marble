#include "katlasflag.h"

#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QPixmapCache>
#include <QtSvg/QSvgRenderer>
#include "katlasdirs.h"

#include <QtCore/QDebug>

KAtlasFlag::KAtlasFlag( QObject* parent ) : QObject(parent) {
	QPixmapCache::setCacheLimit ( 384 );
}

void KAtlasFlag::setFlag( const QString& filename, const QSize& size ){
	m_filename = filename;
	m_size = size;
}

void KAtlasFlag::slotDrawFlag(){

	QString keystring = QString( m_filename ).replace( "flags/", "" );
	if (!QPixmapCache::find( keystring, m_pixmap ) ) {
		QSvgRenderer svgobj( m_filename, this );
		QSize flagsize = svgobj.viewBox().size();
		flagsize.scale(m_size, Qt::KeepAspectRatio);

		m_pixmap = QPixmap( flagsize );
		m_pixmap.fill(Qt::transparent);
		QPainter painter( &m_pixmap );
		painter.setRenderHint(QPainter::Antialiasing, true);
		QRect viewport( QPoint( 0, 0), flagsize );
		painter.setViewport( viewport );
		svgobj.render(&painter);
     		QPixmapCache::insert(keystring, m_pixmap);
	}
	emit flagDone();
}

#ifndef Q_OS_MACX
#include "katlasflag.moc"
#endif
