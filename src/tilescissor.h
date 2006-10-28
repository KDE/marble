//
// C++ Interface: tilescissor
//
// Description: Some class to tile the base map
// functions
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution

#ifndef TILESCISSOR_H
#define TILESCISSOR_H

#include <QObject>
#include <QString>

/**
@author Torsten Rahn
*/

class TileScissor : public QObject {
	Q_OBJECT
public: 
	TileScissor( const QString& prefix, const QString& installmap, const QString& dem );

signals:
	void progress( int value );
public slots:
	void createTiles();

private:
	QString m_prefix, m_installmap, m_dem;

};

#endif
