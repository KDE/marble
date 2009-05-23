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

class AbstractProjectionHelper
{
%TypeHeaderCode
#include <AbstractProjectionHelper.h>
%End

public:
    explicit                AbstractProjectionHelper ();
    virtual void            paintBase (Marble::GeoPainter* painter, Marble::ViewportParams* viewport, QPen& pen, QBrush& brush, bool antialiasing) = 0;
    virtual void            createActiveRegion (Marble::ViewportParams* viewport) = 0;
    void                    setActiveRegion (const QRegion&);
    virtual void            createProjectedRegion (Marble::ViewportParams* viewport) = 0;
    void                    setProjectedRegion (const QRegion&);
    virtual const QRegion   activeRegion () const;
    virtual const QRegion   projectedRegion () const;
    int                     navigationStripe () const;
};   // AbstractProjectionHelper

};   // Marble


