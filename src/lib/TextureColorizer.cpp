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
    const qint64   radius   = viewParams->m_radius;

    const int  imgheight = origimg->height();
    const int  imgwidth  = origimg->width();
    const int  imgrx     = imgwidth / 2;
    const int  imgry     = imgheight / 2;
    // This variable is not used anywhere..
    const int  imgradius = imgrx * imgrx + imgry * imgry;

    const uint landoffscreen = qRgb(255,0,0);
    // const uint seaoffscreen = qRgb(0,0,0);
    const uint lakeoffscreen = qRgb(0,255,0);
    // const uint glaciercolor = qRgb(200,200,200);

    int     bump = 0;
    GpFifo  emboss;
    emboss.buffer = 0;

    const bool showRelief = viewParams->m_showRelief;

    if ( radius * radius > imgradius
         || viewParams->m_projection == Equirectangular )
    {
        int yTop = 0;
        int yBottom = imgheight;

        if( viewParams->m_projection == Equirectangular ) {

            // Calculate translation of center point
            double centerLon, centerLat;
            viewParams->centerCoordinates( centerLon, centerLat );

            int yCenterOffset =  (int)((float)(2*radius / M_PI) * centerLat);
            yTop = ( imgry - radius + yCenterOffset < 0)? 0 : imgry - radius + yCenterOffset;
            yBottom = ( imgry + yCenterOffset + radius > imgheight )? imgheight : imgry + yCenterOffset + radius;
        }

        for (int y = yTop; y < yBottom; ++y) {

            QRgb  *writeData         = (QRgb*)( origimg->scanLine( y ) );
            const QRgb  *coastData   = (QRgb*)( coastimg->scanLine( y ) );

            uchar *readDataStart     = origimg->scanLine( y );
            const uchar *readDataEnd = readDataStart + imgwidth*4;

            emboss.buffer = 0;
		
            for ( uchar* readData = readDataStart; readData < readDataEnd; readData+=4, ++writeData, ++coastData ) {

                // Cheap Embosss / Bumpmapping
                const uchar&  grey = *readData; // qBlue(*data);

                if ( showRelief == true )
                {
                    emboss.gpuint.x4 = grey;
                    emboss.buffer = emboss.buffer >> 8;
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
        int yTop    = ( imgry-radius < 0 ) ? 0 : imgry-radius;
        const int yBottom = ( yTop == 0 ) ? imgheight : imgry + radius;

        for ( int y = yTop; y < yBottom; ++y ) {
            const int  dy = imgry - y;
            int  rx = (int)sqrt( (double)( radius * radius - dy * dy ) );
            int  xLeft  = 0; 
            int  xRight = imgwidth;

            if ( imgrx-rx > 0 ) {
                xLeft  = imgrx - rx; 
                xRight = imgrx + rx;
            }

            QRgb  *writeData         = (QRgb*)( origimg->scanLine( y ) )  + xLeft;
            const QRgb *coastData    = (QRgb*)( coastimg->scanLine( y ) ) + xLeft;

            uchar *readDataStart     = origimg->scanLine( y ) + xLeft * 4;
            const uchar *readDataEnd = origimg->scanLine( y ) + xRight * 4;

 
            for ( uchar* readData = readDataStart; readData < readDataEnd; readData+=4, ++writeData, ++coastData ) {
                // Cheap Embosss / Bumpmapping

                const uchar& grey = *readData; // qBlue(*data);

                if ( showRelief == true )
                {
                    emboss.buffer = emboss.buffer >> 8;
                    emboss.gpuint.x4 = grey;    
                    bump = ( emboss.gpuint.x1 + 16 - grey ) >> 1;

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


