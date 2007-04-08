//
// C++ Interface: TileLoader
//
// Description: TileLoader provides elevation data from a specified file
// (given as a QString) according to the 
// respective input of latitude and longitude. It loads those tiles
// necessary for the given texlevellevel and flushes the tiles on request.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution

#ifndef __MARBLE__TILELOADER_H
#define __MARBLE__TILELOADER_H

#include <QtCore/QHash>
#include <QtGui/QImage>
#include <QtCore/QString>

/**
@author Torsten Rahn
*/

class TextureTile;

class TileLoader {
public:
	TileLoader( const QString& theme );
	virtual ~TileLoader(){}

	TextureTile* loadTile( int tilx, int tily, int tileLevel );

	void setMap( const QString& );

	void resetTilehash();
	void cleanupTilehash();
	void flush();

	const int tileWidth() const { return m_tileWidth; }
	const int tileHeight() const { return m_tileHeight; }

	static int levelToRow( int level );
	static int levelToColumn( int level );
	static int rowToLevel( int row );
	static int columnToLevel( int column );

	// highest level in which all tiles are available
	static int maxCompleteTileLevel( const QString& theme );

	// highest level in which some tiles are available
	static int maxPartialTileLevel( const QString& theme );

	// the mandatory most basic tile level is fully available
	static bool baseTilesAvailable( const QString& theme );

protected:

	TextureTile* m_tile;
	QString m_theme;

	QHash <int, TextureTile*> m_tileHash;
	int m_tileId;

	int m_tileWidth, m_tileHeight;

};

#endif // __MARBLE__TILELOADER_H
