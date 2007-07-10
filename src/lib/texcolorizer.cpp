//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "texcolorizer.h"

#include <QtCore/QString>
#include <QtGui/QColor>

#include <cmath>

#include <QtCore/QDebug>


#include "texturepalette.cpp"


TextureColorizer::TextureColorizer( const QString& seafile, 
                                    const QString& landfile )
{
    Q_UNUSED( seafile );
    Q_UNUSED( landfile );

    m_showRelief = true;
}

void TextureColorizer::colorize(QImage* origimg, const QImage* coastimg, 
                                const int& radius)
{
    const int  imgheight = origimg->height();
    const int  imgwidth  = origimg->width();
    const int  imgrx     = imgwidth / 2;
    const int  imgry     = imgheight / 2;
    const int  imgradius = imgrx * imgrx + imgry * imgry;

    const uint  landoffscreen = qRgb(255,0,0);
    // const uint seaoffscreen = qRgb(0,0,0);
    // const uint glacieroffscreen = qRgb(0,255,0);
    // const uint glaciercolor = qRgb(200,200,200);

    int     bump = 0;
    GpFifo  emboss;
    emboss.buffer = 0;

    double  bendradius = 0;
    const double  bendRelief  = M_PI * 0.5 / ( (double)(radius) * sqrt(2.0) );
    const double  bendReliefx = 0.41 * bendRelief;
    const double  bendReliefm = 0.941246 * bendRelief / bendReliefx;

    const bool showRelief = m_showRelief;

    if ( radius * radius > imgradius ) {
        for (int y = 0; y < imgheight; ++y) {

            QRgb  *data = (QRgb*)( origimg->scanLine( y ) );
            const QRgb  *coastdata = (QRgb*)( coastimg->scanLine( y ) );

            emboss.buffer = 0;
		
            for ( int x = 0; x < imgwidth ; ++x, ++data, ++coastdata ) {

                // Cheap Embosss / Bumpmapping
                const uchar  grey = *data & 0x000000ff; // qBlue(*data);

                emboss.buffer = emboss.buffer >> 8;
                emboss.gpuint.x4 = grey;	

                if ( showRelief == true && emboss.gpuint.x1 > grey )
                    bump = ( emboss.gpuint.x1 - grey ) & 0x0f;
                else
                    bump = 0;

                if ( *coastdata == landoffscreen )
                    *data = texturepalette[bump][grey + 0x100];	
                else {

                    // if (*coastdata == riveroffscreen) {
                    // *data = rivercolor;
                    // }
                    // else {
                    *data = texturepalette[bump][grey];
                    // }
                }	
            }
        }
    }
    else {
        const int  ytop    = ( imgry-radius < 0 ) ? 0 : imgry-radius;
        const int  ybottom = ( ytop == 0 ) ? imgheight : ytop + radius + radius;

        for ( int y = ytop; y < ybottom; ++y ) {

            const int  dy = imgry - y;
            int  rx = (int)sqrt( (double)( radius * radius - dy * dy ) );
            int  xleft  = 0; 
            int  xright = imgwidth;

            if ( imgrx-rx > 0 ) {
                xleft  = imgrx - rx; 
                xright = imgrx + rx;
            }

            QRgb       *data      = (QRgb*)( origimg->scanLine( y ) ) + xleft;
            const QRgb *coastdata = (QRgb*)( coastimg->scanLine( y ) ) + xleft;

            double  relief = imgrx - xleft + bendReliefm * dy ;
 
            for ( int x = xleft;
                  x < xright;
                  ++x, ++data, ++coastdata, relief -= 1.0 )
            {
                // Cheap Embosss / Bumpmapping

                const uchar grey = *data & 0x000000ff; // qBlue(*data);

                emboss.buffer = emboss.buffer >> 8;
                emboss.gpuint.x4 = grey;	

                if ( showRelief == true && emboss.gpuint.x1 > grey ) {
                    bump = ( emboss.gpuint.x1 - grey ) >> 1; // >> 1 to soften bumpmapping

                    // Apply "spherical" bumpmapping 
                    // bump *= cos( bendRelief * sqrt(((imgrx-x)^2+(imgry-y)^2)));

                    // very cheap approximation:
                    // sqrt(dx^2 + dy^2) ~= 0.41 dx + 0.941246  +/- 3%
                    // cos(x) ~= 1-x^2

                    bendradius = bendReliefx * relief;
                    bump *= qRound( 1.0 - bendradius * bendradius );

                    bump &= 0x0f;
                }
                else
                    bump = 0;


                if ( *coastdata == landoffscreen )
                    *data = texturepalette[bump][grey + 0x100];	
                else {
                    // if (*coastdata == riveroffscreen) {
                    // *data = rivercolor;
                    // }
                    // else {
                    *data = texturepalette[bump][grey];
                    // }
                }
            }
        }
    }
}
