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

class GeoDataLabelStyle : Marble::GeoDataColorStyle
{
%TypeHeaderCode
#include <GeoDataLabelStyle.h>
%End


public:
    enum Alignment
    {
        Corner,
        Center
    };

                            GeoDataLabelStyle ();
                            GeoDataLabelStyle (const Marble::GeoDataLabelStyle& other);
                            GeoDataLabelStyle (const QFont& font, const QColor& color);
    void                    setScale (const float& scale /in/);
    float                   scale () const;
    void                    setAlignment (Marble::GeoDataLabelStyle::Alignment alignment);
    Marble::GeoDataLabelStyle::Alignment  alignment () const;
    void                    setFont (const QFont& font);
    QFont                   font () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    ~GeoDataLabelStyle ();
    virtual const char*     nodeType () const;
};
// GeoDataLabelStyle

};
// Marble


