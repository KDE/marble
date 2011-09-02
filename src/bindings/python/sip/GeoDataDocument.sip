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
                            GeoDataDocument ();
                            GeoDataDocument (const Marble::GeoDataDocument& other);
    virtual bool            isGeoDataDocument () const;
    QString                 fileName () const;
    void                    setFileName (const QString& value);
    void                    addStyle (const Marble::GeoDataStyle& style);
    void                    removeStyle (const QString& styleId);
    Marble::GeoDataStyle&   style (const QString& styleId) const;
    QList<Marble::GeoDataStyle>  styles () const;
    void                    addStyleMap (const Marble::GeoDataStyleMap& map);
    void                    removeStyleMap (const QString& mapId);
    Marble::GeoDataStyleMap&  styleMap (const QString& styleId) const;
    QList<Marble::GeoDataStyleMap>  styleMaps () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    ~GeoDataDocument ();
//ig    DocumentRole            documentRole () const;
//ig    void                    setDocumentRole (DocumentRole role);
};
// GeoDataDocument

};
// Marble


