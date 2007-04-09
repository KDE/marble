//
// C++ Interface: texmapper
//
// Description: TextureMapper 

// The TextureMapper maps the Elevationvalues onto the respective projection.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution

#ifndef TEXMAPPER_H
#define TEXMAPPER_H

#include <QtGui/QImage>

#include "Quaternion.h"

/**
@author Torsten Rahn
*/

class TextureTile;
class TileLoader;

class TextureMapper {
public:
	TextureMapper( const QString& path );
	virtual ~TextureMapper();

	void setMap( const QString& path );
	void setMaxTileLevel( int level ){ m_maxTileLevel = level; }
	void resizeMap(const QImage*);
	void mapTexture(QImage*, const int&, Quaternion&);
	void selectTileLevel(const int& radius);

protected:
	void getPixelValueApprox(const float&, const float&, QRgb*);
	void getPixelValue(const float&, const float&, QRgb*);

	int m_posx, m_posy;

	TileLoader* m_tileLoader;
	QRgb* line;
	QRgb* linefast;

	int m_maxTileLevel;
	bool interpolate;
	int nopt;

	int x,y,z;

	float qr, qx, qy, qz;

	int m_imageRadius, m_imageHalfWidth, m_imageHalfHeight;

	float m_prevLat, m_prevLng;
	int m_n; float m_ninv;

	int m_tilxw, m_tilyh;

	int maxfullalpha, maxquatalpha, maxhalfbeta;
	float maxhalfalpha, maxquatbeta;
	int normfullalpha, normhalfbeta;
	float normhalfalpha, normquatbeta;

	float m_rad2pixw, m_rad2pixh;

	TextureTile* m_tile;

	int m_tileLevel;
};

#endif
