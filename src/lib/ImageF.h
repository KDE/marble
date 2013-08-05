//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013        Adrian Draghici <draghici.adrian.b@gmail.com>

#ifndef IMAGEF_H
#define IMAGEF_H

#include <QImage>

namespace Marble
{

class ImageF
{

private:
    ImageF();

public:
    /**
    * @brief Returns the color value of the result tile at a given floating point position.
    * @return The uint that describes the color value of the given pixel
    */
    static uint pixelF( const QImage& image, qreal x, qreal y );

};

}

#endif
