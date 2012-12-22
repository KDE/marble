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
class GeoDataMultiGeometry : Marble::GeoDataGeometry
{
%TypeHeaderCode
#include <GeoDataMultiGeometry.h>
%End

public:
                            GeoDataMultiGeometry ();
                            GeoDataMultiGeometry (const Marble::GeoDataGeometry& other);
    int                     size () const;
//FIXME Add any needed Python list methods.
//ig    Marble::GeoDataGeometry&  at (int pos);
    const Marble::GeoDataGeometry&  at (int pos) const;
//ig    Marble::GeoDataGeometry&  operator [] (int pos);
    const Marble::GeoDataGeometry&  operator [] (int pos) const;
//ig    Marble::GeoDataGeometry&  first ();
    const Marble::GeoDataGeometry&  first () const;
//ig    Marble::GeoDataGeometry&  last ();
    const Marble::GeoDataGeometry&  last () const;
    Marble::GeoDataMultiGeometry&  operator << (const Marble::GeoDataGeometry& value);
//ig    QVector<Marble::GeoDataGeometry>::Iterator  begin ();
//ig    QVector<Marble::GeoDataGeometry>::Iterator  end ();
//ig    QVector<Marble::GeoDataGeometry>::ConstIterator  constBegin () const;
//ig    QVector<Marble::GeoDataGeometry>::ConstIterator  constEnd () const;
    void                    clear ();

//FIXME the result here should be copied.
//ig    QVector<Marble::GeoDataGeometry>&  vector () const;

    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual ~GeoDataMultiGeometry ();
    virtual const Marble::GeoDataLatLonAltBox&  latLonAltBox () const;
    Marble::GeoDataGeometry*  child (int);
    int                     childPosition (Marble::GeoDataGeometry* child);
    void                    append (Marble::GeoDataGeometry* other);
//ig    QVector<GeoDataGeometry*>::Iterator  erase (QVector<GeoDataGeometry*>::Iterator pos);
//ig    QVector<GeoDataGeometry*>::Iterator  erase (QVector<GeoDataGeometry*>::Iterator begin, QVector<GeoDataGeometry*>::Iterator end);
//ig    const Marble::GeoDataGeometry*  child (int) const;
};
// GeoDataMultiGeometry

};
// Marble


