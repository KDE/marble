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

#include <QImage>

#include "quaternion.h"

/**
@author Torsten Rahn
*/

class TextureLoader;

class TextureMapper {
public:
	TextureMapper( const QString& path );
	virtual ~TextureMapper(){}

	void setMap( const QString& path );
	void setMaxTileLevel( int level ){ m_maxtilelevel = level; }
	void resizeMap(const QImage*);
	void mapTexture(QImage*, const int&, Quaternion&);
	void selectTexLevel(const int&);

private:
	TextureLoader* texldr;
	QRgb* line;
	QRgb* linefast;

	int m_maxtilelevel;
	bool interpolate;
	int n;
	float ninv;
	int nopt;

	int x,y,z;
	int rx;
	float qr, qx, qy, qz;

	int imgrx, imgry, imgradius;
	int imgwidth, imgheight;
	int alpha, beta;
	float radalpha, radbeta;
};

#endif
