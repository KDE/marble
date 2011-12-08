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

class GeoDataLineStyle : Marble::GeoDataColorStyle
{
%TypeHeaderCode
#include <GeoDataLineStyle.h>
%End

public:
                            GeoDataLineStyle ();
                            GeoDataLineStyle (const Marble::GeoDataLineStyle& other);
    explicit                GeoDataLineStyle (const QColor& color);
    void                    setWidth (const float& width /in/);
    float                   width () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    ~GeoDataLineStyle ();
    virtual const char*     nodeType () const;
    void                    setPhysicalWidth (const float& realWidth);
    float                   physicalWidth () const;
    void                    setCapStyle (Qt::PenCapStyle style);
    Qt::PenCapStyle         capStyle () const;
    void                    setPenStyle (Qt::PenStyle style);
    Qt::PenStyle            penStyle () const;
    void                    setBackground (bool background);
    bool                    background () const;
    void                    setDashPattern (const QVector<qreal>& pattern);
    QVector<qreal>          dashPattern () const;
};
// GeoDataLineStyle

};
// Marble


