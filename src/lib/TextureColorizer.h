//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// The TextureColorizer maps the Elevationvalues to Legend Colors.
//

#ifndef MARBLE_TEXTURECOLORIZER_H
#define MARBLE_TEXTURECOLORIZER_H

#include <QtCore/QString>

namespace Marble
{

class ViewParams;

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

    void colorize(ViewParams *viewParams);

    QString seafile() const;
    QString landfile() const;

    void generatePalette( const QString& seafile,
                          const QString& landfile );
 private:
    QString m_seafile;
    QString m_landfile;
    static uint texturepalette[16][512];
};

}

#endif
