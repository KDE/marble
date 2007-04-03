//
// C++ Interface: texloader
//
// Description: TextureLoader provides elevation data from a specified file
// (given as a QString) according to the 
// respective input of latitude and longitude. It loads those tiles
// necessary for the given texlevellevel and flushes the tiles on request.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution

#ifndef TEXLOADER_H
#define TEXLOADER_H

#include <QtCore/QHash>
#include <QtGui/QImage>
#include <QtCore/QString>

/**
@author Torsten Rahn
*/

class TextureTile;

class TextureLoader {
public:
	TextureLoader( const QString& );
	virtual ~TextureLoader(){}

	void setMap( const QString& );
	void setTexLevel( const int texlevel );

	void resetTilehash();
	void cleanupTilehash();
	void flush();

	static int levelToRow( int level );
	static int levelToColumn( int level );
	static int rowToLevel( int row );
	static int columnToLevel( int column );

	// highest level in which all tiles are available
	static int maxCompleteTileLevel( QString theme );

	// highest level in which some tiles are available
	static int maxPartialTileLevel( QString theme );

	// the mandatory most basic tile level is fully available
	static bool baseTilesAvailable( QString theme );

//	TODO: These should be moved into TextureMapper:

	void setN( const int n );
	void prePixelValueApprox(const float&, const float&, QRgb*);
	void getPixelValueApprox(const float&, const float&, QRgb*);
	int m_depth;
	const int depth() const { return m_depth; }

protected:
	inline void loadTile();

	TextureTile* m_tile;
	QString m_theme, m_filename;
	QHash <int, TextureTile*> m_tilehash;

	int texpixw, texpixh;
	float rad2pixw, rad2pixh;

	int m_tilxw, m_tilyh;

	int tilx, tily;
	int m_tilw, m_tilh;

	int m_texlevel, m_oldtexlevel, m_tilekey;


//	TODO: These should be moved into TextureMapper:

	void getPixelValue(const float&, const float&, QRgb*);
	float m_prevlat, m_prevlng;
	int m_n; float m_ninv;
	int maxfullalpha, maxquatalpha, maxhalfbeta;
	float maxhalfalpha, maxquatbeta;
	int normfullalpha, normhalfbeta;
	float normhalfalpha, normquatbeta;
	int posx, posy;
};

#endif // 
