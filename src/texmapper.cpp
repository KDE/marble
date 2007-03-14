#include "texmapper.h"

#include <iostream>
#include <QtCore/QTime>
#include <QtGui/QColor>
#include <cmath>

#include "pntmap.h"
#include "katlasdirs.h"
#include "texloader.h"

#include <QtCore/QDebug>

// #define INTERLACE

	const float rad2int = 21600.0 / M_PI;

TextureMapper::TextureMapper( const QString& path ){

	m_maxtilelevel = 1;

	texldr = new TextureLoader( path );

	line = 0;

	x = 0; y = 0; z = 0;
	rx = 0;

 	qr = 0.0f; qx = 0.0f; qy = 0.0f; qz = 0.0f;

	imgheight = 0; imgwidth = 0;
	imgrx = 0; imgry = 0;
	imgradius = 0;

	alpha = 0; beta = 0;
}

void TextureMapper::setMap( const QString& path ){
	texldr->setMap(path);
}

void TextureMapper::selectTexLevel(const int& radius){
	int texlevel=m_maxtilelevel;

	if ( radius < 200 ) texlevel = 1;
	else if ( radius < 675 ) texlevel = 2;
	else if ( radius < 1350 ) texlevel = 4;

	if ( texlevel > m_maxtilelevel ) texlevel = m_maxtilelevel;
	texldr->setTexLevel(texlevel);
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
	selectTexLevel(radius);

	// evaluate the degree of interpolation
	n = (imgradius < radius2) ? nopt : 8;
	ninv = 1.0f / (float)(n);
	texldr->setN(n);

	// Calculate the actual y-range of the map on the screen 
	const int ytop = (imgry-radius < 0) ? 0 : imgry-radius;
	const int ybottom = (ytop == 0) ? imgheight : ytop + radius + radius;

//	Quaternion* qpos = ( FastMath::haveSSE() == true )? new QuaternionSSE() : new Quaternion();
	Quaternion* qpos = new Quaternion();

//	calculate north pole position to decrease pole distortion later on
	GeoPoint northpole( 0.0f, (float)( -M_PI*0.5 ) );
	Quaternion qpolepos = northpole.getQuatPoint();
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
		int xipright = (int)(imgwidth * ninv) * n; 

		if (imgrx-rx > 0){
			xipleft = n * (int)(xleft/n + 1);
			xipright = n * (int)(xright/n - 1);
		}

//		decrease pole distortion due to linear approximation ( y-axis )
		bool poleyenv = false;
		int northpoley = imgry + (int)( radius * qpolepos.v[Q_Y] );
		if ( qpolepos.v[Q_Z] > 0 &&  northpoley - n/2 <= y && northpoley + n/2 >= y ){
			poleyenv = true;
		}

		int ncount = 0;
		for (x = xleft; x < xright; x++){
			// Prepare for interpolation

			if ((x >= xipleft) && (x <= xipright)){

//				decrease pole distortion due to linear approximation ( x-axis )
				int northpolex = imgrx + (int)( radius * qpolepos.v[Q_X] );

				int leftinterval = xipleft + ncount * n;
				if ( poleyenv == true &&  northpolex > leftinterval  && northpolex  < leftinterval + n && x  < leftinterval + n){
					interpolate = false;
				}
				else{
					x+=n-1;
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

			//Approx for n-1 out of n pixels within the boundary of xipleft to xipright
			if (interpolate) {
				texldr->getPixelValueApprox(radalpha,radbeta,line);
#ifdef INTERLACE
				for (int j=0; j < n-1; j++){ linefast[j]=line[j]; }; linefast +=(n-1);
#endif
				line +=(n-1);
			}
			texldr->prePixelValueApprox(radalpha,radbeta,line);
#ifdef INTERLACE
			*linefast=*line; linefast++;
#endif
			line++;
		}
	}
	delete qpos;	

	texldr->cleanupTilehash();
}
