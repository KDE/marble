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
    void                    append (const Marble::GeoDataCoordinates& position);
    Marble::GeoDataLineString&  operator << (const Marble::GeoDataCoordinates& position);
//ig    QVector<Marble::GeoDataCoordinates>::Iterator  begin ();
//ig    QVector<Marble::GeoDataCoordinates>::Iterator  end ();
//ig    QVector<Marble::GeoDataCoordinates>::ConstIterator  constBegin () const;
//ig    QVector<Marble::GeoDataCoordinates>::ConstIterator  constEnd () const;
    void                    clear ();
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);

protected:
//ig    Marble::GeoDataLineStringPrivate*  p () const;

public:
    virtual ~GeoDataLineString ();
    virtual QString         nodeType () const;
    virtual qreal           length (qreal planetRadius, int offset = 0) const;
//ig    virtual QVector<Marble::GeoDataLineString*>  toRangeCorrected () const;
    virtual Marble::GeoDataLineString  toNormalized () const;
    virtual Marble::GeoDataLineString  toPoleCorrected () const;
//ig    virtual QVector<Marble::GeoDataLineString*>  toDateLineCorrected () const;
    bool                    isEmpty () const;
//ig    Marble::GeoDataLineString&  operator << (const Marble::GeoDataLineString& lineString);
//ig    QVector<GeoDataCoordinates>::Iterator  erase (QVector<GeoDataCoordinates>::Iterator position);
//ig    QVector<GeoDataCoordinates>::Iterator  erase (QVector<GeoDataCoordinates>::Iterator begin, QVector<GeoDataCoordinates>::Iterator end);
    void                    remove (int i);
protected:
//ig                            GeoDataLineString (GeoDataLineStringPrivate* priv);
};
// GeoDataLineString

};
// Marble
