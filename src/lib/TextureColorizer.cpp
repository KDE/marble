//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Carlos Licea <carlos.licea@kdemail.net>
//

#include "TextureColorizer.h"

#include <cmath>

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtGui/QColor>

#include "global.h"
#include "ViewParams.h"

uint TextureColorizer::texturepalette[16][512];

TextureColorizer::TextureColorizer( const QString& seafile, 
                                    const QString& landfile )
{
//    Q_UNUSED( seafile );
//    Q_UNUSED( landfile );

   generatePalette(seafile, landfile);
}

void TextureColorizer::colorize(ViewParams *viewParams)
{
    QImage  *origimg  = viewParams->m_canvasImage;
    const QImage  *coastimg = viewParams->m_coastImage;
    const qint64   radius   = viewParams->radius();

    const int  imgheight = origimg->height();
    const int  imgwidth  = origimg->width();
    const int  imgrx     = imgwidth / 2;
    const int  imgry     = imgheight / 2;
    // This variable is not used anywhere..
    const int  imgradius = imgrx * imgrx + imgry * imgry;

    const uint landoffscreen = qRgb(255,0,0);
    // const uint seaoffscreen = qRgb(0,0,0);
    const uint lakeoffscreen = qRgb(0,0,0);
    // const uint glaciercolor = qRgb(200,200,200);

    int     bump = 0;
    GpFifo  emboss;
    emboss.buffer = 0;

    const bool showRelief = viewParams->m_showRelief;

    if ( radius * radius > imgradius
         || viewParams->projection() == Equirectangular
         || viewParams->projection() == Mercator )
    {
        int yTop = 0;
        int yBottom = imgheight;

        if( viewParams->projection() == Equirectangular
            || viewParams->projection() == Mercator ) {

            // Calculate translation of center point
            double centerLon, centerLat;
            viewParams->centerCoordinates( centerLon, centerLat );

            int yCenterOffset =  (int)((float)(2*radius / M_PI) * centerLat);
            if ( viewParams->projection() == Equirectangular ) {
                yTop = ( imgry - radius + yCenterOffset < 0)? 0 : imgry - radius + yCenterOffset;
                yBottom = ( imgry + yCenterOffset + radius > imgheight )? imgheight : imgry + yCenterOffset + radius;
            }
            else if ( viewParams->projection() == Mercator ) {
                yTop = ( imgry - 2 * ( yCenterOffset + radius ) < 0)? 0 : imgry - 2 * ( radius -   yCenterOffset );
                yBottom = ( imgry + 2 * ( yCenterOffset + radius ) > imgheight )? imgheight : imgry + 2 * ( yCenterOffset + radius );
            }
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

                int red  = qRed( *coastData );
                if ( red == 255 || red == 0 ) {
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
                else
                {
                    if ( qRed( *coastData ) != 0 && qGreen( *coastData ) == 0) {
                        double landalpha = (double)(red) / 255.0;

                        QRgb landcolor  = (QRgb)(texturepalette[bump][grey + 0x100]);
                        QRgb watercolor = (QRgb)(texturepalette[bump][grey]);

                        *writeData = qRgb( 
                              (int)( landalpha * qRed( landcolor ) )
                            + (int)( ( 1-landalpha ) * qRed( watercolor ) ),
                              (int)( landalpha * qGreen( landcolor ) )
                            + (int)( ( 1-landalpha ) * qGreen( watercolor ) ),
                              (int)( landalpha * qBlue( landcolor ) )
                            + (int)( ( 1-landalpha ) * qBlue( watercolor ) )
                        );
                    }
                    else {

                        if ( qGreen( *coastData ) != 0 ) {
                            double landalpha = qGreen(*coastData) / 255.0;

                            QRgb landcolor  = (QRgb)(texturepalette[bump][grey + 0x100]);
                            QRgb glaciercolor = (QRgb)(texturepalette[bump][grey]);
    
                            *writeData = qRgb( 
                                (int)( landalpha * qRed( glaciercolor ) )
                                + (int)( ( 1-landalpha ) * qRed( landcolor ) ),
                                (int)( landalpha * qGreen( glaciercolor ) )
                                + (int)( ( 1-landalpha ) * qGreen( landcolor ) ),
                                (int)( landalpha * qBlue( glaciercolor ) )
                                + (int)( ( 1-landalpha ) * qBlue( landcolor ) )
                            ); 
                        }

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

                int red  = qRed( *coastData );
                if ( red == 255 || red == 0 ) {
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
                else
                {
                    if ( qRed( *coastData ) != 0 && qGreen( *coastData ) == 0) {
                        double landalpha = (double)(red) / 255.0;

                        QRgb landcolor  = (QRgb)(texturepalette[bump][grey + 0x100]);
                        QRgb watercolor = (QRgb)(texturepalette[bump][grey]);

                        *writeData = qRgb( 
                              (int)( landalpha * qRed( landcolor ) )
                            + (int)( ( 1-landalpha ) * qRed( watercolor ) ),
                              (int)( landalpha * qGreen( landcolor ) )
                            + (int)( ( 1-landalpha ) * qGreen( watercolor ) ),
                              (int)( landalpha * qBlue( landcolor ) )
                            + (int)( ( 1-landalpha ) * qBlue( watercolor ) )
                        );
                    }
                    else {

                        if ( qGreen( *coastData ) != 0 ) {
                            double landalpha = qGreen(*coastData) / 255.0;

                            QRgb landcolor  = (QRgb)(texturepalette[bump][grey + 0x100]);
                            QRgb glaciercolor = (QRgb)(texturepalette[bump][grey]);
    
                            *writeData = qRgb( 
                                (int)( landalpha * qRed( glaciercolor ) )
                                + (int)( ( 1-landalpha ) * qRed( landcolor ) ),
                                (int)( landalpha * qGreen( glaciercolor ) )
                                + (int)( ( 1-landalpha ) * qGreen( landcolor ) ),
                                (int)( landalpha * qBlue( glaciercolor ) )
                                + (int)( ( 1-landalpha ) * qBlue( landcolor ) )
                            ); 
                        }

                    }
                }
            }
        }
    }
}

void TextureColorizer::generatePalette(const QString& seafile, const QString& landfile) const
{    

    //Text copy/pasted from tools/palettegen.cpp 

    QImage  *gradimg = new QImage( 256, 10, QImage::Format_RGB32 );

    QStringList  filelist;
    filelist << seafile << landfile;
    QString  filename;

    QPainter  painter(gradimg);
    painter.setPen(Qt::NoPen);

    for ( int j = 0; j < 16; ++j ) {
  
        int offset = 0;

        foreach ( filename, filelist ) {
	
            QLinearGradient  gradient( 0, 0, 256, 0 );

            QFile  file( filename );
            file.open( QIODevice::ReadOnly );
            QTextStream  stream( &file );  // read the data serialized from the file
            QString  evalstrg;

            while ( !stream.atEnd() ) {
                stream >> evalstrg;
                if ( !evalstrg.isEmpty() && evalstrg.contains( "=" ) ) {
                    QString  colval = evalstrg.section( "=", 0, 0 );
                    QString  colpos = evalstrg.section( "=", 1, 1 );
                    gradient.setColorAt(colpos.toDouble(), QColor(colval));
                }
            }
            painter.setBrush( gradient );
            painter.drawRect( 0, 0, 256, 10 );	

            int  alpha = j;

            for ( int i = 0; i < 256; ++i) {

                    QRgb  shadeColor = gradimg->pixel( i, 1 );
                    QImage  shadeImage ( 256, 10, QImage::Format_RGB32 );
                    QLinearGradient  shadeGradient( 0, 0, 256, 0 );
                    shadeGradient.setColorAt(0.15, QColor(Qt::white));
                    shadeGradient.setColorAt(0.496, shadeColor);
                    shadeGradient.setColorAt(0.504, shadeColor);
                    shadeGradient.setColorAt(0.75, QColor(Qt::black));
                    QPainter  shadePainter(&shadeImage);
                    shadePainter.setPen(Qt::NoPen);
                    shadePainter.setBrush( shadeGradient );
                    shadePainter.drawRect( 0, 0, 256, 10 );  
                    int shadeIndex = 120 + alpha;
//                    qDebug() << QString("Shade: %1").arg(shadeIndex);
                    QRgb  palcol = shadeImage.pixel( shadeIndex, 1 );

                // populate texturepalette[][]
                texturepalette[j][offset + i] = (uint)palcol;
            }

            offset += 256;
        }
    }
}
