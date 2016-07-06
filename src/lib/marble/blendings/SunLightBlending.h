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

#include <QtGlobal>

#include "Blending.h"

namespace Marble
{

class SunLocator;

class SunLightBlending: public Blending
{
 public:
    explicit SunLightBlending( const SunLocator * sunLocator );
    virtual ~SunLightBlending();
    virtual void blend( QImage * const bottom, TextureTile const * const top ) const;

    void setLevelZeroLayout( int levelZeroColumns, int levelZeroRows );

 private:
    static int maxDivisor( int maximum, int fullLength );
    const SunLocator * const m_sunLocator;
    int m_levelZeroColumns;
    int m_levelZeroRows;
};

}

#endif
