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
#include <marble/GeoDataObject.h>
%End

public:
    int                     id () const;
    void                    setId (int value);
    int                     targetId () const;
    void                    setTargetId (int value);
    Marble::GeoDataObject*  parent ();
    void                    setParent (Marble::GeoDataObject*);
    virtual Marble::GeoDataObject*  child (int);
    int                     row ();
    virtual int             childPosition (Marble::GeoDataObject*);
    virtual int             childCount ();
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& steam);

protected:
    explicit                GeoDataObject (Marble::GeoDataObject* parent = 0);
                            GeoDataObject (const Marble::GeoDataObject&);
};   // GeoDataObject

};   // Marble


