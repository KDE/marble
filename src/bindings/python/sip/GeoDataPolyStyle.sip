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

class GeoDataPolyStyle : Marble::GeoDataColorStyle
{
%TypeHeaderCode
#include <GeoDataPolyStyle.h>
%End

public:
                            GeoDataPolyStyle ();
                            GeoDataPolyStyle (const Marble::GeoDataPolyStyle& other);
    explicit                GeoDataPolyStyle (const QColor& color);

    void                    setFill (const bool& fill /in/);
    bool                    fill () const;
    void                    setOutline (const bool& outline /in/);
    bool                    outline () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    ~GeoDataPolyStyle ();
    virtual const char*     nodeType () const;
};
// GeoDataPolyStyle

};
// Marble


