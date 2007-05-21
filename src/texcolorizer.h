//
// C++ Interface: texcolorizer
//
// Description: TextureColorizer 

// The TextureColorizer maps the Elevationvalues to Legend Colors.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution


#ifndef TEXCOLORIZER_H
#define TEXCOLORIZER_H


#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtGui/QImage>


/**
@author Torsten Rahn
*/


typedef struct
{
    uchar  x1;
    uchar  x2;
    uchar  x3;
    uchar  x4;
} GpUint;


typedef union
{
    uint    buffer;
    GpUint  gpuint;
} GpFifo;

class TextureColorizer
{
 public:
    TextureColorizer(const QString&, const QString&);
    virtual ~TextureColorizer(){}
    void colorize(QImage*, const QImage*, const int&);

 private:
    static const uint  texturepalette[][512];
};


#endif
