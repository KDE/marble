//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn     <tackat@kde.org>"
//


#ifndef SPHERICALSCANLINETEXTUREMAPPER_H
#define SPHERICALSCANLINETEXTUREMAPPER_H


#include <QtCore/QString>
#include <QtGui/QColor>

#include "AbstractScanlineTextureMapper.h"


class TileLoader;


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
    explicit SphericalScanlineTextureMapper( TileLoader *tileLoader, QObject * parent =0 );
    virtual ~SphericalScanlineTextureMapper();

    void resizeMap(int width, int height);
    void mapTexture( ViewParams *viewParams );

 protected:
    void pixelValueApprox(const double& lon, const double& lat,
                          QRgb *scanLine, bool smooth );

    bool         m_interpolate;
    int          m_nBest;

    int     m_n;
    double  m_nInverse;
};


#endif // SPHERICALSCANLINETEXTUREMAPPER_H
