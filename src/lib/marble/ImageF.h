// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>

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
    static uint pixelF(const QImage &image, qreal x, qreal y);
};

}

#endif
