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

class LatLonEdit : QWidget
{
%TypeHeaderCode
#include <LatLonEdit.h>
%End


public:
    explicit                LatLonEdit (QWidget* parent /TransferThis/ = 0, Marble::Dimension dimension = Marble::Longitude, Marble::LatLonEdit::Notation notation = Marble::LatLonEdit::DMS);
    qreal                   value () const;

//FIXME implementation is missing.
//ig    Marble::Dimension       dimension () const;
    void                    setValue (qreal newvalue);
    void                    setDimension (Marble::Dimension dimension);

signals:
    void                    valueChanged (qreal value);
public:
    ~LatLonEdit ();
    enum Notation
    {
        Decimal,
        DMS,
        DM
    };
    Marble::LatLonEdit::Notation  notation () const;
    void                    setNotation (Marble::LatLonEdit::Notation notation);
};
// LatLonEdit

};
// Marble


