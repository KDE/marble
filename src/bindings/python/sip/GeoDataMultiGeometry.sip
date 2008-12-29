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
class GeoDataMultiGeometry : /* QVector<GeoDataGeometry*>, */ Marble::GeoDataGeometry
{
%TypeHeaderCode
#include <GeoDataMultiGeometry.h>
%End

public:
    explicit                GeoDataMultiGeometry (Marble::GeoDataObject* parent = 0);
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual EnumGeometryId  geometryId () const;
//------ QVector methods
    void                    append(Marble::GeoDataGeometry* value);
    Marble::GeoDataGeometry* at(int i) const;
    void                    clear();
    bool                    contains(Marble::GeoDataGeometry* value) const;
    int                     count(Marble::GeoDataGeometry* value) const;
    int                     count() const;
    void                    fill(Marble::GeoDataGeometry* value, int size = -1);
    Marble::GeoDataGeometry* first();
    int                     indexOf(Marble::GeoDataGeometry* value, int from = 0) const;
    void                    insert(int i, Marble::GeoDataGeometry* value);
    bool                    isEmpty() const;
    Marble::GeoDataGeometry* last();
    int                     lastIndexOf(Marble::GeoDataGeometry* value, int from = -1) const;
//    Marble::GeoDataLineString mid(int pos, int length = -1) const;

    void                    prepend(Marble::GeoDataGeometry* value);
    void                    push_back(Marble::GeoDataGeometry* value);
    void                    push_front(Marble::GeoDataGeometry* value);

    void                    remove(int i);
    void                    remove(int i, int count);
    void                    replace(int i, Marble::GeoDataGeometry* value);
    int                     size() const;
    Marble::GeoDataGeometry* value(int i) const;
    Marble::GeoDataGeometry* value(int i, Marble::GeoDataGeometry* defaultValue) const;
    bool                    operator!=(const Marble::GeoDataMultiGeometry &other) const;

//Marble::GeoDataLineString operator+(const Marble::GeoDataLineString &other) const;
    Marble::GeoDataMultiGeometry &operator+=(const Marble::GeoDataMultiGeometry &other);
    Marble::GeoDataMultiGeometry &operator+=(Marble::GeoDataMultiGeometry* value);

    bool                    operator==(const Marble::GeoDataMultiGeometry &other) const;

    Marble::GeoDataGeometry &operator[](int i);
%MethodCode
SIP_SSIZE_T idx = sipConvertFromSequenceIndex(a0, sipCpp->count());
if (idx < 0) {
    sipIsErr = 1;
} else {
//    sipRes = new Marble::GeoDataGeometry(*sipCpp->operator[]((int)idx));
    sipRes = sipCpp->operator[]((int)idx);

}
%End

    int                     __len__();
%MethodCode
    sipRes = sipCpp->count();
%End

    void                    __setitem__(int i, Marble::GeoDataGeometry*value);
%MethodCode
int len;

len = sipCpp->count();

if ((a0 = (int)sipConvertFromSequenceIndex(a0, len)) < 0) {
    sipIsErr = 1;
} else {
    (*sipCpp)[a0] = a1;
}
%End

    void                    __setitem__(SIP_PYSLICE slice, const Marble::GeoDataMultiGeometry &list);
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
        QVector<Marble::GeoDataGeometry*>::const_iterator it = a1->begin();

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

    Marble::GeoDataMultiGeometry &operator[](SIP_PYSLICE slice);
%MethodCode
SIP_SSIZE_T len, start, stop, step, slicelength, i;
len = sipCpp->count();

if (PySlice_GetIndicesEx((PySliceObject *)a0, len, &start, &stop, &step, &slicelength) < 0) {
    sipIsErr = 1;
} else {
    sipRes = new Marble::GeoDataMultiGeometry();

    for (i = 0; i < slicelength; ++i) {
        (*sipRes) += (*sipCpp)[start];
        start += step;
    }
}
%End

       int __contains__(Marble::GeoDataGeometry* value);
%MethodCode
sipRes = bool(sipCpp->contains(a0));
%End

//------

private:
                            GeoDataMultiGeometry (const Marble::GeoDataMultiGeometry&);
};   // GeoDataMultiGeometry

};   // Marble


