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

class GeoDataContainer : Marble::GeoDataFeature
{
%TypeHeaderCode
#include <GeoDataContainer.h>
%End


public:
                            GeoDataContainer ();
                            GeoDataContainer (const Marble::GeoDataContainer& other);
                            GeoDataContainer (const Marble::GeoDataFeature& other);
//ig    Marble::GeoDataContainerPrivate*  p () const;
//    QVector<Marble::GeoDataFolder>  folders () const;
    QVector<Marble::GeoDataFeature>&  features () const;
//    QVector<Marble::GeoDataPlacemark>  placemarks () const;
    void                    append (const Marble::GeoDataFeature& other);
    int                     size () const;
//ig    Marble::GeoDataFeature&  at (int pos);
    const Marble::GeoDataFeature&  at (int pos) const;
//ig    Marble::GeoDataFeature&  last ();
    const Marble::GeoDataFeature&  last () const;
//ig    Marble::GeoDataFeature&  first ();
    const Marble::GeoDataFeature&  first () const;
//ig    QVector<Marble::GeoDataFeature>::Iterator  begin ();
//ig    QVector<Marble::GeoDataFeature>::Iterator  end ();
//ig    QVector<Marble::GeoDataFeature>::ConstIterator  constBegin () const;
//ig    QVector<Marble::GeoDataFeature>::ConstIterator  constEnd () const;
    void                    clear ();
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);

private:
//ig                            GeoDataContainer (Marble::GeoDataContainerPrivate* priv);

public:
    virtual Marble::GeoDataObject*  child (int);
    virtual int             childPosition (Marble::GeoDataObject*);
    virtual int             childCount ();
};   // GeoDataContainer

};   // Marble


