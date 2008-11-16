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

class GeoDataDocument : Marble::GeoDocument, Marble::GeoDataContainer
{
%TypeHeaderCode
#include <GeoDataDocument.h>
%End

public:
    explicit                GeoDataDocument (Marble::GeoDataObject* parent = 0);
    virtual EnumFeatureId   featureId () const;
    virtual bool            isGeoDataDocument () const;
    void                    addStyle (Marble::GeoDataStyle* style);
    void                    removeStyle (Marble::GeoDataStyle* style);
    Marble::GeoDataStyle*   style (const QString& styleId);
    QList<Marble::GeoDataStyle*>  styles () const;
//FIXME    void                    addStyleMap (Marble::GeoDataStyleMap* map);
//FIXME    void                    removeStyleMap (Marble::GeoDataStyleMap* map);
//FIXME    Marble::GeoDataStyleMap*  styleMap (const QString& styleId);
//FIXME    QList<Marble::GeoDataStyleMap*>  styleMaps () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);

private:
                            GeoDataDocument (const Marble::GeoDataDocument&);
};   // GeoDataDocument

};   // Marble


