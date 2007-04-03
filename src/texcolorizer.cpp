#include "texcolorizer.h"

#include <QtGui/QColor>
#include <QtCore/QString>
#include <QtGui/QPainter>
#include <QtGui/QGradient>
#include <QtCore/QFile>

#include <cmath>

#include <QtCore/QDebug>


#include "texturepalette.cpp"

TextureColorizer::TextureColorizer(QString seafile, QString landfile){
}

void TextureColorizer::colorize(QImage* origimg, const QImage* coastimg, const int& radius){

	const int imgheight = origimg->height();
	const int imgwidth = origimg->width();
	const int imgrx = imgwidth >> 1;
	const int imgry = imgheight >> 1;
	const int imgradius = imgrx*imgrx + imgry*imgry;

	const uint landoffscreen = qRgb(255,0,0);
//	const uint seaoffscreen = qRgb(0,0,0);
//	const uint glacieroffscreen = qRgb(0,255,0);
//	const uint glaciercolor = qRgb(200,200,200);

	int bump = 0;
	GpFifo emboss;
	emboss.buffer = 0;

	float bendradius = 0;
	const float bendRelief = M_PI * 0.5f / ( (float)(radius) * sqrtf(2.0f) );
	const float bendReliefx = 0.41f * bendRelief;
	const float bendReliefm = 0.941246f * bendRelief / bendReliefx;

	if ( radius*radius > imgradius ){
		for (int y = 0; y < imgheight; y++){

			QRgb* data = (QRgb*)(origimg->scanLine( y ));
			const QRgb* coastdata = (QRgb*)(coastimg->scanLine( y ));

			emboss.buffer = 0;
		
			for (int x = 0; x < imgwidth ; ++x, ++data, ++coastdata){

				// Cheap Embosss / Bumpmapping

				const uchar grey = *data & 0x000000ff; // qBlue(*data);

				emboss.buffer = emboss.buffer >> 8;
				emboss.gpuint.x4 = grey;	

				if (emboss.gpuint.x1 > grey)
					bump = (emboss.gpuint.x1 - grey) & 0x0f;
				else
					bump = 0;


				if (*coastdata == landoffscreen)
					*data=TextureColorizer::texturepalette[bump][grey + 0x100];	
				else {
//					if (*coastdata == riveroffscreen){
//						*data = rivercolor;
//					}
//					else {
						*data = TextureColorizer::texturepalette[bump][grey];
//					}
				}	
			}
		}
	}
	else {
		const int ytop = (imgry-radius < 0) ? 0 : imgry-radius;
		const int ybottom = (ytop == 0) ? imgheight : ytop + radius + radius;

		for (int y = ytop; y < ybottom; y++){


			const int dy = imgry - y;
			int rx = (int)sqrtf((float)(radius * radius - dy*dy));
			int xleft = 0; int xright = imgwidth;

			if (imgrx-rx > 0){
				xleft = imgrx - rx; xright = imgrx + rx;
			}

			QRgb* data = (QRgb*)(origimg->scanLine( y )) + xleft;
			const QRgb* coastdata = (QRgb*)(coastimg->scanLine( y )) + xleft;

			float relief = imgrx -xleft + bendReliefm * dy ;
			for (int x = xleft; x < xright; ++x, ++data, ++coastdata, relief-=1.0f){


				// Cheap Embosss / Bumpmapping

				const uchar grey = *data & 0x000000ff; // qBlue(*data);

				emboss.buffer = emboss.buffer >> 8;
				emboss.gpuint.x4 = grey;	

				if (emboss.gpuint.x1 > grey){
					bump = (emboss.gpuint.x1 - grey) >> 1; // >> 1 to soften bumpmapping

//					Apply "spherical" bumpmapping 
//					bump *= cos( bendRelief * sqrt(((imgrx-x)^2+(imgry-y)^2)));

//					very cheap approximation:
//					 sqrt(dx^2 + dy^2) ~= 0.41 dx + 0.941246  +/- 3%
//					 cos(x) ~= 1-x^2

					bendradius = bendReliefx * relief;
					bump *= qRound( 1.0f - bendradius * bendradius );

					bump &= 0x0f;
				}
				else
					bump = 0;


				if (*coastdata == landoffscreen)
					*data=TextureColorizer::texturepalette[bump][grey + 0x100];	
				else {
//					if (*coastdata == riveroffscreen){
//						*data = rivercolor;
//					}
//					else {
						*data = TextureColorizer::texturepalette[bump][grey];
//					}
				}	
			}
		}

	}

}
