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

class GeoDataColorStyle : Marble::GeoDataObject
{
%TypeHeaderCode
#include <GeoDataColorStyle.h>
%End

public:
    void                    setColor (const QColor& value);
    QColor                  color () const;

    enum ColorMode
    {
        Normal,
        Random
    };

    void                    setColorMode (const Marble::GeoDataColorStyle::ColorMode& colorMode);
    Marble::GeoDataColorStyle::ColorMode  colorMode () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
                            GeoDataColorStyle ();
                            GeoDataColorStyle (const Marble::GeoDataColorStyle& other);
    virtual ~GeoDataColorStyle ();
    virtual const char*     nodeType () const;
};
// GeoDataColorStyle

};
// Marble


