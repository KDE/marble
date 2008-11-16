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

class GeoDataStyle : Marble::GeoDataStyleSelector
{
%TypeHeaderCode
#include <GeoDataStyle.h>
%End

public:
                            GeoDataStyle ();
                            GeoDataStyle (const QPixmap& icon, const QFont& font, const QColor& color);
    void                    setIconStyle (Marble::GeoDataIconStyle* style);
    Marble::GeoDataIconStyle*  iconStyle ();
    void                    setLabelStyle (Marble::GeoDataLabelStyle* style);
    Marble::GeoDataLabelStyle*  labelStyle ();
    void                    setLineStyle (Marble::GeoDataLineStyle* style);
    Marble::GeoDataLineStyle*  lineStyle ();
    void                    setPolyStyle (Marble::GeoDataPolyStyle* style);
    Marble::GeoDataPolyStyle*  polyStyle ();
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);

private:
                            GeoDataStyle (const Marble::GeoDataStyle&);
};   // GeoDataStyle

};   // Marble


