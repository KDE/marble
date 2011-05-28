// Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_SUN_LIGHT_BLENDING_H
#define MARBLE_SUN_LIGHT_BLENDING_H

#include <QtCore/QtGlobal>
#include <QtGui/QColor>

#include "Blending.h"

namespace Marble
{

class SunLocator;

class SunLightBlending: public Blending
{
 public:
    SunLightBlending();
    virtual ~SunLightBlending();
    virtual void blend( QImage * const bottom, TextureTile const * const top ) const;

 private:
    void shadePixelComposite( QRgb & bottom, QRgb const top, qreal const brightness ) const;
    qreal shading( qreal const deltaLon, qreal const a, qreal const c ) const;
    int maxDivisor( int const maximum, int const fullLength ) const;
    SunLocator * const m_sunLocator;
};

}

#endif
