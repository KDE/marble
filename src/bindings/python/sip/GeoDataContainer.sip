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
//ig    Marble::GeoDataContainerPrivate*  p () const;
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


    virtual ~GeoDataContainer ();
private:
//ig                            GeoDataContainer (GeoDataContainerPrivate* priv);
public:
//ig    QVector<Marble::GeoDataFolder*>  folderList () const;
//ig    QVector<Marble::GeoDataFeature*>  featureList () const;
//ig    QVector<Marble::GeoDataPlacemark*>  placemarkList () const;
    Marble::GeoDataFeature*  child (int);
    int                     childPosition (Marble::GeoDataFeature* child);
    void                    append (Marble::GeoDataFeature* other);
    void                    remove (int index);
    Marble::GeoDataLatLonAltBox  latLonAltBox () const;
//ig    const Marble::GeoDataFeature*  child (int) const;
};
// GeoDataContainer

};
// Marble


