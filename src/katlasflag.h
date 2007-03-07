//
// C++ Interface: gpmapscale
//
// Description: KAtlasFlag 

// KAtlasFlag paints a flag on a pixmap
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASFLAG_H
#define KATLASFLAG_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

/**
@author Torsten Rahn
*/

class KAtlasFlag : public QObject {

Q_OBJECT

public:
	KAtlasFlag( QObject *parent = 0 );

	QPixmap& flag(){ return m_pixmap; }
	void setFlag( QString filename, QSize size );

public slots:
	void slotDrawFlag();

signals:
	void flagDone();

protected:
	QString m_filename;
	QSize m_size;
	QPixmap m_pixmap;
};

#endif // KATLASFLAG_H
