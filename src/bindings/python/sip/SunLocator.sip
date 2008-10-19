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
#include <marble/SunLocator.h>
%End

public:
//FIXME    explicit                SunLocator (Marble::ExtDateTime* dateTime);
    qreal                   shading (qreal lon, qreal lat);
    void                    shadePixel (QRgb& pixcol /in,out/, qreal shade);
    void                    shadePixelComposite (QRgb& pixcol /in,out/, QRgb& dpixcol /in/, qreal shade);
    void                    setShow (bool show);
    void                    setCitylights (bool show);
    void                    setCentered (bool show);
    bool                    getShow ();
    bool                    getCitylights ();
    bool                    getCentered ();
    qreal                   getLon ();
    qreal                   getLat ();
//FIXME    Marble::ExtDateTime*    datetime ();
    void                    update ();

signals:
    void                    updateStars ();
    void                    updateSun ();
    void                    centerSun ();
    void                    reenableWidgetInput ();

protected:
                            SunLocator (const Marble::SunLocator&);
};   // SunLocator

};   // Marble


