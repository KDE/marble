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

#include "TextureColorizer.h"

#include <cmath>

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtGui/QColor>

#include "global.h"
#include "ViewParams.h"
#include "texturepalette.cpp"


TextureColorizer::TextureColorizer( const QString& seafile, 
                                    const QString& landfile )
{
    Q_UNUSED( seafile );
    Q_UNUSED( landfile );
}

void TextureColorizer::colorize(ViewParams *viewParams)
{
    QImage  *origimg  = viewParams->m_canvasImage;
    const QImage  *coastimg = viewParams->m_coastImage;
    const int      radius   = viewParams->m_radius;

    const int  imgheight = origimg->height();
    const int  imgwidth  = origimg->width();
    const int  imgrx     = imgwidth / 2;
    const int  imgry     = imgheight / 2;
    // This variable is not used anywhere..
    const int  imgradius = imgrx * imgrx + imgry * imgry;

    const uint  landoffscreen = qRgb(255,0,0);
    // const uint seaoffscreen = qRgb(0,0,0);
    const uint lakeoffscreen = qRgb(0,255,0);
    // const uint glaciercolor = qRgb(200,200,200);

    int     bump = 0;
    GpFifo  emboss;
    emboss.buffer = 0;

    double  bendradius = 0.0;
    const double  bendRelief  = M_PI * 0.5 / ( (double)(radius) * sqrt(2.0) );
    const double  bendReliefx = 0.41 * bendRelief;
    const double  bendReliefm = 0.941246 * bendRelief / bendReliefx;

    const bool showRelief = viewParams->m_showRelief;

    if ( radius * radius > imgradius || viewParams->m_projection == Equirectangular) {
        int yTop = 0;
        int yBottom = imgheight;

        if( viewParams->m_projection == Equirectangular ) {

            // Calculate translation of center point
            double centerLat =  viewParams->m_planetAxis.pitch() + M_PI;
            if ( centerLat > M_PI ) centerLat -= 2 * M_PI; 

            int yCenterOffset =  (int)((float)(2*radius / M_PI) * centerLat);
            yTop = ( imgry - radius + yCenterOffset < 0)? 0 : imgry - radius + yCenterOffset;
            yBottom = ( imgry + yCenterOffset + radius > imgheight )? imgheight : imgry + yCenterOffset + radius;
        }

        for (int y = yTop; y < yBottom; ++y) {

            QRgb  *writeData = (QRgb*)( origimg->scanLine( y ) );
            uchar  *readData = origimg->scanLine( y );
            const QRgb  *coastData = (QRgb*)( coastimg->scanLine( y ) );

            emboss.buffer = 0;
		
            for ( int x = 0; x < imgwidth ; ++x, ++writeData, ++coastData, readData+=4 ) {

                // Cheap Embosss / Bumpmapping
                const uchar  grey = *readData; // qBlue(*data);

                emboss.buffer = emboss.buffer >> 8;
                emboss.gpuint.x4 = grey;	

                if ( showRelief == true )
                {
                    bump = ( emboss.gpuint.x1 + 8 - grey );
                    if ( bump  < 0 )  bump = 0;
                    if ( bump  > 15 )  bump = 15;
                }
                else
                    bump = 8;

                if ( *coastData == landoffscreen )
                    *writeData = texturepalette[bump][grey + 0x100]; 
                else {
                    if (*coastData == lakeoffscreen)
                    *writeData = texturepalette[bump][0x055];
                    else {
                        *writeData = texturepalette[bump][grey];
                    }
                }
            }
        }
    }
    else {
        int ytop    = ( imgry-radius < 0 ) ? 0 : imgry-radius;
        int ybottom = ( ytop == 0 ) ? imgheight : imgry + radius;

        for ( int y = ytop; y < ybottom; ++y ) {
            const int  dy = imgry - y;
            int  rx = (int)sqrt( (double)( radius * radius - dy * dy ) );
            int  xleft  = 0; 
            int  xright = imgwidth;

            if ( imgrx-rx > 0 ) {
                xleft  = imgrx - rx; 
                xright = imgrx + rx;
            }

            QRgb  *writeData      = (QRgb*)( origimg->scanLine( y ) ) + xleft;
            uchar  *readData          = origimg->scanLine( y ) + xleft*4;
            const QRgb *coastData = (QRgb*)( coastimg->scanLine( y ) ) + xleft;

            double  relief = imgrx - xleft + bendReliefm * dy ;
 
            for ( int x = xleft;
                  x < xright;
                  ++x, ++writeData, ++coastData, readData+=4, relief -= 1.0 )
            {
                // Cheap Embosss / Bumpmapping

                const uchar grey = *readData; // qBlue(*data);

                emboss.buffer = emboss.buffer >> 8;
                emboss.gpuint.x4 = grey;	

                if ( showRelief == true )
                {
                    bump = ( emboss.gpuint.x1 + 8 - grey ) >> 1;
                    bump += 3;
/*
                    // Apply "spherical" bumpmapping 
                    // bump *= cos( bendRelief * sqrt(((imgrx-x)^2+(imgry-y)^2)));

                    // very cheap approximation:
                    // sqrt(dx^2 + dy^2) ~= 0.41 dx + 0.941246  +/- 3%
                    // cos(x) ~= 1-x^2
                    bendradius = bendReliefx * relief;
                    bump *= qRound( 1.0 - bendradius * bendradius );
*/
                    if ( bump > 15 ) bump = 15;
                    if ( bump < 0 ) bump = 0;
                }
                else
                    bump = 8;

                if ( *coastData == landoffscreen )
                    *writeData = texturepalette[bump][grey + 0x100];	
                else {
                    if (*coastData == lakeoffscreen)
                    *writeData = texturepalette[bump][0x055];
                    else {
                        *writeData = texturepalette[bump][grey];
                    }
                }
            }
        }
    }
}


