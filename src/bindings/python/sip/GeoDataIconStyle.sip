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

class GeoDataIconStyle : Marble::GeoDataColorStyle
{
%TypeHeaderCode
#include <GeoDataIconStyle.h>
%End


public:
                            GeoDataIconStyle ();
                            GeoDataIconStyle (const Marble::GeoDataIconStyle& other);
    QImage                 icon () const;
    void                    setIconPath (const QString& filename);
    void                    setHotSpot (const QPointF& hotSpot, Marble::GeoDataHotSpot::Units xunits, Marble::GeoDataHotSpot::Units yunits);
    const QPointF&          hotSpot () const;
    void                    setScale (const float& scale /in/);
    float                   scale () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    ~GeoDataIconStyle ();
    virtual const char*     nodeType () const;
    explicit                GeoDataIconStyle (const QImage& icon, const QPointF& hotSpot = QPointF(0.5,0.5));
    void                    setIcon (const QImage& icon);
    QString                 iconPath () const;
//ig    QPointF                 hotSpot (Marble::GeoDataHotSpot::Units& xunits, Marble::GeoDataHotSpot::Units& yunits) const;
};
// GeoDataIconStyle

};
// Marble


