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

class GeoDataLineString : Marble::GeoDataGeometry
{
%TypeHeaderCode
#include <GeoDataLineString.h>
%End

public:
//ig    typedef QVector<GeoDataCoordinates>::Iterator Iterator;

//ig    typedef QVector<GeoDataCoordinates>::ConstIterator ConstIterator;

                            GeoDataLineString (Marble::TessellationFlags f = Marble::NoTessellation);
                            GeoDataLineString (const Marble::GeoDataGeometry& other);
    virtual bool            isClosed () const;
    bool                    tessellate () const;
    void                    setTessellate (bool tessellate);
    Marble::TessellationFlags  tessellationFlags () const;
    void                    setTessellationFlags (Marble::TessellationFlags f);
    Marble::GeoDataLatLonAltBox  latLonAltBox () const;
    int                     size () const;
//FIXME Add the needed Python list methods.
//ig    Marble::GeoDataCoordinates&  at (int pos);
    const Marble::GeoDataCoordinates&  at (int pos) const;
//ig    Marble::GeoDataCoordinates&  operator [] (int pos);
    const Marble::GeoDataCoordinates&  operator [] (int pos) const;
//ig    Marble::GeoDataCoordinates&  first ();
    const Marble::GeoDataCoordinates&  first () const;
//ig    Marble::GeoDataCoordinates&  last ();
    const Marble::GeoDataCoordinates&  last () const;
    void                    append (const Marble::GeoDataCoordinates& value);
    Marble::GeoDataLineString&  operator << (const Marble::GeoDataCoordinates& value);
//ig    QVector<Marble::GeoDataCoordinates>::Iterator  begin ();
//ig    QVector<Marble::GeoDataCoordinates>::Iterator  end ();
//ig    QVector<Marble::GeoDataCoordinates>::ConstIterator  constBegin () const;
//ig    QVector<Marble::GeoDataCoordinates>::ConstIterator  constEnd () const;
    void                    clear ();
//ig    QVector<Marble::GeoDataCoordinates>::Iterator  erase (QVector<Marble::GeoDataCoordinates>::Iterator pos);
//ig    QVector<Marble::GeoDataCoordinates>::Iterator  erase (QVector<Marble::GeoDataCoordinates>::Iterator begin, QVector<Marble::GeoDataCoordinates>::Iterator end);
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);

protected:
//ig    Marble::GeoDataLineStringPrivate*  p () const;
//ig                                       GeoDataLineString (Marble::GeoDataLineStringPrivate* priv);

public:
                            GeoDataLineString (const Marble::GeoDataLineString&);
    virtual EnumGeometryId  geometryId () const;
};   // GeoDataLineString

};   // Marble
