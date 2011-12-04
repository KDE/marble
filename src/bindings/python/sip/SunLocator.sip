//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

namespace Marble
{

class SunLocator : QObject /Abstract/
{
%TypeHeaderCode
#include <SunLocator.h>
%End


public:
    qreal                   shading (qreal lon, qreal a, qreal c) const;
    qreal                   getLon () const;
    qreal                   getLat () const;
    void                    update ();


    virtual ~SunLocator ();
    void                    shadePixel (QRgb& pixcol, qreal shade) const;
    void                    shadePixelComposite (QRgb& pixcol, const QRgb& dpixcol, qreal shade) const;
//ig                            SunLocator (const MarbleClock* clock, const Marble::Planet* planet);
    void                    setPlanet (const Marble::Planet* planet);
signals:
    void                    positionChanged (qreal lon, qreal lat);
};
// SunLocator

};
// Marble


