#include <cmath>
#include <iostream>

#include <QtCore/QTime>
#include <QtGui/QColor>

#include "GeoPoint.h"
#include "GeoPolygon.h"
#include "katlasdirs.h"
#include "texturetile.h"
#include "texloader.h"
#include "texmapper.h"


#include <QtCore/QDebug>

// #define INTERLACE

const float rad2int = 2160000.0f / M_PI;
const float TWOPI = 2 * M_PI;

TextureMapper::TextureMapper( const QString& path ){

	m_maxtilelevel = 0;

	texldr = new TextureLoader( path );

	line = 0;

	x = 0; y = 0; z = 0;
	rx = 0;

 	qr = 0.0f; qx = 0.0f; qy = 0.0f; qz = 0.0f;

	imgheight = 0; imgwidth = 0;
	imgrx = 0; imgry = 0;
	imgradius = 0;

	radalpha = 0.0f; radbeta = 0.0f;

	m_tileLevel = 0;
}

TextureMapper::~TextureMapper(){
	delete texldr;
}

void TextureMapper::setMap( const QString& path ){

	texldr->setMap(path);

}

void TextureMapper::selectTileLevel(const int& radius){


	float linearlevel = (float) radius / 335.0f;
	int tileLevel = 0;
	if (linearlevel > 0.0f )
		tileLevel = (int)( logf( linearlevel ) / logf( 2.0f ) ) + 1;

	if ( tileLevel > m_maxtilelevel ) tileLevel = m_maxtilelevel;

//	texldr->setTileLevel(tileLevel);
	if ( tileLevel != m_tileLevel ){
		texldr->flush();
		m_tileLevel = tileLevel;
	}

	int texpixw = (int)(4320000.0f / (float)( TextureLoader::levelToColumn( tileLevel ) ) / (float)(texldr->tileWidth()));
	int texpixh = (int)(2160000.0f / (float)( TextureLoader::levelToRow( tileLevel ) ) / (float)(texldr->tileHeight()));

	m_rad2pixw = (2160000.0f / M_PI / (float)(texpixw));
	m_rad2pixh = (2160000.0f / M_PI / (float)(texpixh));


	maxfullalpha = (int)(4320000.0f / (float)(texpixw)) - 1;
	maxhalfalpha = (float)(2160000.0f / (float)(texpixw));
	maxquatalpha = (int)(1080000.0f / (float)(texpixw));
	maxquatbeta = (float)(1080000.0f / (float)(texpixh));
	maxhalfbeta = (int) ( 2.0f * maxquatbeta) - 1;

	normhalfalpha = maxhalfalpha - m_tilxw;
	normquatbeta = maxquatbeta - m_tilyh;
	normfullalpha = maxfullalpha - m_tilxw;
	normhalfbeta = maxhalfbeta - m_tilyh;

//	qDebug() << "Texture Level was set to: " << tileLevel;
}

void TextureMapper::resizeMap(const QImage* origimg){
	imgwidth = origimg -> width();
	imgheight = origimg -> height();
	imgrx = imgwidth >> 1;
	imgry = imgheight >> 1;
	imgradius = imgrx*imgrx + imgry*imgry;

	nopt=2;
	int nevalmin = imgwidth;
	for (int it = 1; it < 32; it++){
		int neval = imgwidth/it + imgwidth%it;
		if ( neval < nevalmin ) {
			nevalmin = neval;
			nopt = it; 
		}
	}
//	qDebug("Optimized n = " + QString::number(nopt).toLatin1());
}

void TextureMapper::mapTexture(QImage* origimg, const int& radius, Quaternion& rotAxis){
// Scanline based algorithm to texture map a sphere

	const int radius2 = radius*radius;
	const float radiusf = 1.0f/(float)(radius);

	texldr->resetTilehash();

	m_tilxw = 65535;
	m_tilyh = 65535;

	normhalfalpha = maxhalfalpha - m_tilxw;
	normquatbeta = maxquatbeta - m_tilyh;
	normfullalpha = maxfullalpha - m_tilxw;
	normhalfbeta = maxhalfbeta - m_tilyh;
// -- snip --

	selectTileLevel(radius);

	// evaluate the degree of interpolation
	m_n = (imgradius < radius2) ? nopt : 8;
	m_ninv = 1.0f / (float)(m_n);

	// Calculate the actual y-range of the map on the screen 
	const int ytop = (imgry-radius < 0) ? 0 : imgry-radius;
	const int ybottom = (ytop == 0) ? imgheight : ytop + radius + radius;

//	Quaternion* qpos = ( FastMath::haveSSE() == true )? new QuaternionSSE() : new Quaternion();
	Quaternion* qpos = new Quaternion();

//	calculate north pole position to decrease pole distortion later on
	GeoPoint northpole( 0.0f, (float)( -M_PI*0.5 ) );
	Quaternion qpolepos = northpole.quaternion();
	Quaternion invRotAxis = rotAxis;
	invRotAxis = invRotAxis.inverse();

	qpolepos.rotateAroundAxis(invRotAxis);

	matrix rotMatrix;
	rotAxis.toMatrix( rotMatrix );

#ifndef INTERLACE
	for (y = ytop; y < ybottom; y++){
#else
	for (y = ytop; y < ybottom -1; y+=2){
#endif
		// Evaluate coordinates for the 3D position vector of the current pixel
		qy = radiusf * (float)(y - imgry);
		qr = 1.0f - qy*qy;

		// rx is the radius component in x direction
		rx = (int)sqrtf((float)(radius2 - (y - imgry)*(y - imgry)));

		// Calculate the actual x-range of the map within the current scanline
		const int xleft = (imgrx-rx > 0) ? imgrx - rx : 0; 
		const int xright = (imgrx-rx > 0) ? xleft + rx + rx : imgwidth;

		line = (QRgb*)(origimg->scanLine( y )) + xleft;
#ifdef INTERLACE
		linefast = (QRgb*)(origimg->scanLine( y + 1 )) + xleft;
#endif		

		int xipleft = 1;
		int xipright = (int)(imgwidth * m_ninv) * m_n; 

		if (imgrx-rx > 0){
			xipleft = m_n * (int)(xleft/m_n + 1);
			xipright = m_n * (int)(xright/m_n - 1);
		}

//		decrease pole distortion due to linear approximation ( y-axis )
		bool poleyenv = false;
		int northpoley = imgry + (int)( radius * qpolepos.v[Q_Y] );
		if ( qpolepos.v[Q_Z] > 0 &&  northpoley - m_n/2 <= y && northpoley + m_n/2 >= y ){
			poleyenv = true;
		}

		int ncount = 0;

		for (x = xleft; x < xright; x++){
			// Prepare for interpolation

			if ((x >= xipleft) && (x <= xipright)){

//				decrease pole distortion due to linear approximation ( x-axis )
				int northpolex = imgrx + (int)( radius * qpolepos.v[Q_X] );

				int leftinterval = xipleft + ncount * m_n;
				if ( poleyenv == true &&  northpolex > leftinterval  && northpolex  < leftinterval + m_n && x  < leftinterval + m_n){
					interpolate = false;
				}
				else{
					x+=m_n-1;
					interpolate=true;
					ncount++;
				} 
			}
			else 
				interpolate = false;

			// Evaluate more coordinates for the 3D position vector of the current pixel
			qx = (float)(x - imgrx) * radiusf;

			float qr2z = qr - qx*qx;
			qz = (qr2z > 0.0f) ? sqrtf(qr2z) : 0.0f;	

			// Create Quaternion from vector coordinates and rotate it around globe axis
			qpos->set(0,qx,qy,qz);
			qpos->rotateAroundAxis( rotMatrix );	

			qpos->getSpherical(radalpha, radbeta);

//			if (radbeta < 0) qpos->display(); 

			//Approx for m_n-1 out of n pixels within the boundary of xipleft to xipright
			if (interpolate) {
				getPixelValueApprox(radalpha,radbeta,line);
#ifdef INTERLACE
				for (int j=0; j < m_n-1; j++){ linefast[j]=line[j]; }; linefast +=(m_n-1);
#endif
				line +=(m_n-1);
			}
			prePixelValueApprox(radalpha,radbeta,line);
#ifdef INTERLACE
			*linefast=*line; linefast++;
#endif
			line++;
		}
	}
	delete qpos;	
//	qDebug() << "end";
	texldr->cleanupTilehash();
}

void TextureMapper::getPixelValueApprox(const float& lng, const float& lat, QRgb* line){
//	This method executes the interpolation for skipped pixels in a scanline.
//	We rather might want to move this into TextureMapper.

	float avglat = ( lat-m_prevlat ) * m_ninv;
	float avglng = lng-m_prevlng;

	if (fabs(avglng) > M_PI){

		avglng = ( TWOPI - fabs(avglng) ) * m_ninv;

		if (m_prevlng < lng){
			for (int j=1; j < m_n; j++){
				m_prevlat += avglat;
				m_prevlng -= avglng;
				if (m_prevlng <= -M_PI) m_prevlng += TWOPI;
				getPixelValue( m_prevlng, m_prevlat, line );
				line++;
			}
		}
		// if (m_prevlng > lng)
		else { 
			float curAvgLng = lng - m_n*avglng;
			for (int j=1; j < m_n; j++){
				m_prevlat += avglat;
				curAvgLng += avglng;
				float evallng = curAvgLng;
				if (curAvgLng <= -M_PI) evallng += TWOPI;
				getPixelValue( evallng, m_prevlat, line);
				line++;
			}
		}
	}

	else {

		avglng *= m_ninv;
		for (int j=1; j < m_n; j++) {
			m_prevlat += avglat;
			m_prevlng += avglng;
			getPixelValue( m_prevlng, m_prevlat, line);
			line++;
		}
	}	
}

inline void TextureMapper::prePixelValueApprox(const float& radlng, const float& radlat, QRgb* line){
//	This method prepares the interpolation for skipped pixels in a scanline.
//	We rather might move this into TextureMapper.

	m_prevlat = radlat;
	m_prevlng = radlng;
	getPixelValue(radlng, radlat, line);
}

inline void TextureMapper::getPixelValue(const float& radlng, const float& radlat, QRgb* line){

//	Convert lng and lat measured in radiant 
//	to pixel position on the current m_tile ...
	m_posx = (int)(normhalfalpha + radlng * m_rad2pixw);
	m_posy = (int)(normquatbeta + radlat * m_rad2pixh);

//	qDebug() << "Width: " << texldr->tileWidth() << " Height: " << texldr->tileHeight();

	if ( m_posx >= texldr->tileWidth() || m_posx < 0 || m_posy >= texldr->tileHeight() || m_posy < 0 )
	{
		// necessary to prevent e.g. crash if TextureMapper::radalpha = -pi
		if ( m_posx > normfullalpha ) m_posx = normfullalpha;
		if ( m_posy > normhalfbeta  ) m_posy = normhalfbeta;

//		The origin (0, 0) is in the upper left corner
//		lng: 360 deg = 43200 pixel
//		lat: 180 deg = 21600 pixel
		int lng = m_posx + m_tilxw;
		int lat = m_posy + m_tilyh;

		int tilx = lng / texldr->tileWidth(); // Counts the m_tiles left from the current m_tile ("m_tileposition") 
		int tily = lat / texldr->tileHeight(); // Counts the m_tiles on the top from the current m_tile

		m_tilxw = tilx * texldr->tileWidth();
		m_tilyh = tily * texldr->tileHeight();

		normhalfalpha = maxhalfalpha - m_tilxw;
		normquatbeta = maxquatbeta - m_tilyh;
		normfullalpha = maxfullalpha - m_tilxw;
		normhalfbeta = maxhalfbeta - m_tilyh;

		m_tile = texldr->loadTile( tilx, tily, m_tileLevel );

		m_posx = lng - m_tilxw;
		m_posy = lat - m_tilyh;
	}

	if (m_tile->depth() == 8)
		*line = m_tile->jumpTable8[m_posy][m_posx];
	else
		*line = m_tile->jumpTable32[m_posy][m_posx];
}

