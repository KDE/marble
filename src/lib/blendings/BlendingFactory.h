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

#ifndef MARBLE_BLENDING_FACTORY_H
#define MARBLE_BLENDING_FACTORY_H

#include <QtCore/QHash>
#include <QtCore/QString>

namespace Marble
{
class Blending;
class SunLightBlending;
class SunLocator;

class BlendingFactory
{
 public:
    BlendingFactory( SunLocator *sunLocator );
    ~BlendingFactory();

    void setLevelZeroLayout( int levelZeroColumns, int levelZeroRows );

    Blending const * findBlending( QString const & name ) const;

 private:
    SunLightBlending *const m_sunLightBlending;
    QHash<QString, Blending const *> m_blendings;
};

}

#endif
