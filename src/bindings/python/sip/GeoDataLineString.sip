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
class GeoDataLineString : /* QVector<Marble::GeoDataCoordinates*>, */ Marble::GeoDataGeometry
{
%TypeHeaderCode
#include <GeoDataLineString.h>
%End

public:
    explicit                GeoDataLineString (Marble::GeoDataObject* parent = 0, Marble::TessellationFlags f = Marble::NoTessellation);
                            GeoDataLineString (const Marble::GeoDataLineString&);
    virtual bool            isClosed () const;
    bool                    tessellate () const;
    void                    setTessellate (bool tessellate);
    Marble::TessellationFlags  tessellationFlags () const;
    void                    setTessellationFlags (Marble::TessellationFlags f);
    Marble::GeoDataLatLonAltBox  latLonAltBox () const;
    void                    append (Marble::GeoDataCoordinates* value);
    void                    clear ();

//ig     QVector<Marble::GeoDataCoordinates*>::Iterator  erase (QVector<Marble::GeoDataCoordinates*>::Iterator pos);
//ig     QVector<Marble::GeoDataCoordinates*>::Iterator  erase (QVector<Marble::GeoDataCoordinates*>::Iterator begin, QVector<Marble::GeoDataCoordinates*>::Iterator end);

    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual EnumGeometryId  geometryId () const;

//------ QVector methods
// append() is already mentioned above.
    Marble::GeoDataCoordinates* at(int i) const;
// clear() is already mentioned above.
    bool                    contains(Marble::GeoDataCoordinates* value) const;
    int                     count(Marble::GeoDataCoordinates* value) const;
    int                     count() const;
    void                    fill(Marble::GeoDataCoordinates* value, int size = -1);
    Marble::GeoDataCoordinates* first();
    int                     indexOf(Marble::GeoDataCoordinates* value, int from = 0) const;
    void                    insert(int i, Marble::GeoDataCoordinates* value);
    bool                    isEmpty() const;
    Marble::GeoDataCoordinates* last();
    int                     lastIndexOf(Marble::GeoDataCoordinates* value, int from = -1) const;
//    Marble::GeoDataLineString mid(int pos, int length = -1) const;

    void                    prepend(Marble::GeoDataCoordinates* value);
    void                    push_back(Marble::GeoDataCoordinates* value);
    void                    push_front(Marble::GeoDataCoordinates* value);

    void                    remove(int i);
    void                    remove(int i, int count);
    void                    replace(int i, Marble::GeoDataCoordinates* value);
    int                     size() const;
    Marble::GeoDataCoordinates* value(int i) const;
    Marble::GeoDataCoordinates* value(int i, Marble::GeoDataCoordinates* defaultValue) const;
    bool                    operator!=(const Marble::GeoDataLineString &other) const;

//Marble::GeoDataLineString operator+(const Marble::GeoDataLineString &other) const;
    Marble::GeoDataLineString &operator+=(const Marble::GeoDataLineString &other);
    Marble::GeoDataLineString &operator+=(Marble::GeoDataCoordinates* value);

    bool                    operator==(const Marble::GeoDataLineString &other) const;

    Marble::GeoDataCoordinates &operator[](int i);
%MethodCode
SIP_SSIZE_T idx = sipConvertFromSequenceIndex(a0, sipCpp->count());
if (idx < 0) {
    sipIsErr = 1;
} else {
    sipRes = new Marble::GeoDataCoordinates(*sipCpp->operator[]((int)idx));
}
%End

    int                     __len__();
%MethodCode
    sipRes = sipCpp->count();
%End

    void                    __setitem__(int i, Marble::GeoDataCoordinates*value);
%MethodCode
int len;

len = sipCpp->count();

if ((a0 = (int)sipConvertFromSequenceIndex(a0, len)) < 0) {
    sipIsErr = 1;
} else {
    (*sipCpp)[a0] = a1;
}
%End

    void                    __setitem__(SIP_PYSLICE slice, const Marble::GeoDataLineString &list);
%MethodCode
SIP_SSIZE_T len, start, stop, step, slicelength, i;

len = sipCpp->count();
if (PySlice_GetIndicesEx((PySliceObject *)a0, len, &start, &stop, &step, &slicelength) < 0) {
    sipIsErr = 1;
} else {
    int vlen = a1->count();

    if (vlen != slicelength) {
        sipBadLengthForSlice(vlen, slicelength);
        sipIsErr = 1;
    } else {
        QVector<Marble::GeoDataCoordinates*>::const_iterator it = a1->begin();

        for (i = 0; i < slicelength; ++i) {
            (*sipCpp)[start] = *it;
            start += step;
            ++it;
        }
    }
}
%End

    void                    __delitem__(int i);
%MethodCode
int len;

len = sipCpp->count();

if ((a0 = (int)sipConvertFromSequenceIndex(a0, len)) < 0) {
    sipIsErr = 1;
} else {
    sipCpp->remove(a0);
}
%End

void __delitem__(SIP_PYSLICE slice);
%MethodCode
SIP_SSIZE_T len, start, stop, step, slicelength, i;

len = sipCpp->count();

if (PySlice_GetIndicesEx((PySliceObject *)a0, len, &start, &stop, &step, &slicelength) < 0) {
    sipIsErr = 1;
} else {
    for (i = 0; i < slicelength; ++i) {
        sipCpp->remove(start);
        start += step - 1;
    }
}
%End

    Marble::GeoDataLineString &operator[](SIP_PYSLICE slice);
%MethodCode
SIP_SSIZE_T len, start, stop, step, slicelength, i;
len = sipCpp->count();

if (PySlice_GetIndicesEx((PySliceObject *)a0, len, &start, &stop, &step, &slicelength) < 0) {
    sipIsErr = 1;
} else {
    sipRes = new Marble::GeoDataLineString();

    for (i = 0; i < slicelength; ++i) {
        (*sipRes) += (*sipCpp)[start];
        start += step;
    }
}
%End

       int __contains__(Marble::GeoDataCoordinates* value);
%MethodCode
sipRes = bool(sipCpp->contains(a0));
%End

//------

};   // GeoDataLineString

};   // Marble
