//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
//


#ifndef __MARBLE__FLATSCANLINETEXTUREMAPPER_H
#define __MARBLE__FLATSCANLINETEXTUREMAPPER_H


#include <QtCore/QString>

#include "AbstractScanlineTextureMapper.h"


class FlatScanlineTextureMapper : public AbstractScanlineTextureMapper
{
 public:
    explicit FlatScanlineTextureMapper(TileLoader *tileLoader, QObject * parent = 0);
    void mapTexture( ViewParams *viewParams );

 private:
    float  m_oldCenterLon;
    int    m_oldYPaintedTop;
};


#endif
