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

struct TileContainer{
	TileContainer(const QString&);
	virtual ~TileContainer();

	QImage* rawtile;

	bool used;
	int width;
	int height;
	uchar **jumpTable8;
	uint **jumpTable32;
	int depth;
};

class TextureLoader {
public:
	TextureLoader( const QString& );
	virtual ~TextureLoader(){}

	void setMap( const QString& );
	void resetTilehash();
	void cleanupTilehash();
	void prePixelValueApprox(const float&, const float&, QRgb*);
	void getPixelValueApprox(const float&, const float&, QRgb*);
	void setN( const int n );

	void flush();
	void setTexLevel( const int texlevel );
protected:
	void getPixelValue(const float&, const float&, QRgb*);
	inline void loadTile();

	TileContainer* tile;
	QString m_fileprefix, m_filename;
	QHash <int, TileContainer*> tilehash;
	int m_oldlat;
	float m_prevlat, m_prevlng;
	int m_n; float m_ninv;

	float avglng, avglat;
	int texpixw, texpixh;
	float rad2pixw, rad2pixh;
	int maxfullalpha, maxquatalpha, maxhalfbeta;
	float maxhalfalpha, maxquatbeta;

	int tilx, tily;
	int tilxw, tilyh;
	int tilw, tilh;
	int posx, posy;

	int m_texlevel, m_oldtexlevel, tilekey;
};

#endif
