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

class GeoDataObject : Marble::GeoNode, Serializable
{
%TypeHeaderCode
#include <GeoDataObject.h>
%End

public:
                            GeoDataObject ();
                            GeoDataObject (const Marble::GeoDataObject&);
    int                     id () const;
    void                    setId (int value);
    int                     targetId () const;
    void                    setTargetId (int value);
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& steam);
    virtual ~GeoDataObject ();
    virtual const char*     nodeType () const;
    virtual Marble::GeoDataObject*  parent () const;
    virtual void            setParent (Marble::GeoDataObject* parent);
};
// GeoDataObject

};
// Marble


