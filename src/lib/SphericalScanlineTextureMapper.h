//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn     <tackat@kde.org>
//

#ifndef MARBLE_SPHERICALSCANLINETEXTUREMAPPER_H
#define MARBLE_SPHERICALSCANLINETEXTUREMAPPER_H


#include <QtGui/QColor>

#include "AbstractScanlineTextureMapper.h"


namespace Marble
{

class StackedTileLoader;


/*
 * @short Texture mapping onto a sphere
 *
 * This class provides a fast way to map textures onto a sphere
 * without making use of hardware acceleration. 
 *
 * @author Torsten Rahn <rahn@kde.org>
 */

class SphericalScanlineTextureMapper : public AbstractScanlineTextureMapper
{
 public:
    explicit SphericalScanlineTextureMapper( StackedTileLoader *tileLoader, QObject *parent = 0 );

    void mapTexture( ViewParams *viewParams );

 protected:
    inline bool needsFilter( const QRgb& rgb, int& oldR, int& oldB, int &oldG  );
};

}

#endif
