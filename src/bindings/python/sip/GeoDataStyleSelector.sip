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

class GeoDataStyleSelector : Marble::GeoDataObject
{
%TypeHeaderCode
#include <GeoDataStyleSelector.h>
%End

public:
    void                    setStyleId (const QString& value);
    QString                 styleId () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
                            GeoDataStyleSelector ();
                            GeoDataStyleSelector (const Marble::GeoDataStyleSelector& other);
    ~GeoDataStyleSelector ();
    virtual const char*     nodeType () const;
};
// GeoDataStyleSelector

};
// Marble


