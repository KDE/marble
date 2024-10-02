// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#include "GeoDataMultiGeometry.h"
#include "GeoDataMultiGeometry_p.h"

#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataTypes.h"

#include "MarbleDebug.h"

#include <QDataStream>

namespace Marble
{

GeoDataMultiGeometry::GeoDataMultiGeometry()
    : GeoDataGeometry(new GeoDataMultiGeometryPrivate)
{
}

GeoDataMultiGeometry::GeoDataMultiGeometry(const GeoDataGeometry &other)
    : GeoDataGeometry(other)
{
}

GeoDataMultiGeometry::~GeoDataMultiGeometry() = default;

const char *GeoDataMultiGeometry::nodeType() const
{
    return GeoDataTypes::GeoDataMultiGeometryType;
}

EnumGeometryId GeoDataMultiGeometry::geometryId() const
{
    return GeoDataMultiGeometryId;
}

GeoDataGeometry *GeoDataMultiGeometry::copy() const
{
    return new GeoDataMultiGeometry(*this);
}

bool GeoDataMultiGeometry::operator==(const GeoDataMultiGeometry &other) const
{
    Q_D(const GeoDataMultiGeometry);
    const GeoDataMultiGeometryPrivate *const other_d = other.d_func();
    QList<GeoDataGeometry *>::const_iterator thisBegin = d->m_vector.constBegin();
    QList<GeoDataGeometry *>::const_iterator thisEnd = d->m_vector.constEnd();
    QList<GeoDataGeometry *>::const_iterator otherBegin = other_d->m_vector.constBegin();
    QList<GeoDataGeometry *>::const_iterator otherEnd = other_d->m_vector.constEnd();

    for (; thisBegin != thisEnd && otherBegin != otherEnd; ++thisBegin, ++otherBegin) {
        if (**thisBegin != **otherBegin) {
            return false;
        }
    }

    return true;
}

const GeoDataLatLonAltBox &GeoDataMultiGeometry::latLonAltBox() const
{
    Q_D(const GeoDataMultiGeometry);

    QList<GeoDataGeometry *>::const_iterator it = d->m_vector.constBegin();
    QList<GeoDataGeometry *>::const_iterator end = d->m_vector.constEnd();

    d->m_latLonAltBox.clear();
    for (; it != end; ++it) {
        if (!(*it)->latLonAltBox().isEmpty()) {
            if (d->m_latLonAltBox.isEmpty()) {
                d->m_latLonAltBox = (*it)->latLonAltBox();
            } else {
                d->m_latLonAltBox |= (*it)->latLonAltBox();
            }
        }
    }
    return d->m_latLonAltBox;
}

int GeoDataMultiGeometry::size() const
{
    Q_D(const GeoDataMultiGeometry);
    return d->m_vector.size();
}

QList<GeoDataGeometry *> GeoDataMultiGeometry::vector()
{
    Q_D(const GeoDataMultiGeometry);

    return d->m_vector;
}

GeoDataGeometry &GeoDataMultiGeometry::at(int pos)
{
    mDebug() << "detaching!";
    detach();

    Q_D(GeoDataMultiGeometry);
    return *(d->m_vector[pos]);
}

const GeoDataGeometry &GeoDataMultiGeometry::at(int pos) const
{
    Q_D(const GeoDataMultiGeometry);
    return *(d->m_vector.at(pos));
}

GeoDataGeometry &GeoDataMultiGeometry::operator[](int pos)
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return *(d->m_vector[pos]);
}

const GeoDataGeometry &GeoDataMultiGeometry::operator[](int pos) const
{
    Q_D(const GeoDataMultiGeometry);
    return *(d->m_vector[pos]);
}

GeoDataGeometry &GeoDataMultiGeometry::last()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return *(d->m_vector.last());
}

GeoDataGeometry &GeoDataMultiGeometry::first()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return *(d->m_vector.first());
}

const GeoDataGeometry &GeoDataMultiGeometry::last() const
{
    Q_D(const GeoDataMultiGeometry);
    return *(d->m_vector.last());
}

const GeoDataGeometry &GeoDataMultiGeometry::first() const
{
    Q_D(const GeoDataMultiGeometry);
    return *(d->m_vector.first());
}

QList<GeoDataGeometry *>::Iterator GeoDataMultiGeometry::begin()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return d->m_vector.begin();
}

QList<GeoDataGeometry *>::Iterator GeoDataMultiGeometry::end()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return d->m_vector.end();
}

QList<GeoDataGeometry *>::ConstIterator GeoDataMultiGeometry::constBegin() const
{
    Q_D(const GeoDataMultiGeometry);
    return d->m_vector.constBegin();
}

QList<GeoDataGeometry *>::ConstIterator GeoDataMultiGeometry::constEnd() const
{
    Q_D(const GeoDataMultiGeometry);
    return d->m_vector.constEnd();
}

/**
 * @brief  returns the requested child item
 */
GeoDataGeometry *GeoDataMultiGeometry::child(int i)
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return d->m_vector.at(i);
}

const GeoDataGeometry *GeoDataMultiGeometry::child(int i) const
{
    Q_D(const GeoDataMultiGeometry);
    return d->m_vector.at(i);
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataMultiGeometry::childPosition(const GeoDataGeometry *object) const
{
    Q_D(const GeoDataMultiGeometry);
    for (int i = 0; i < d->m_vector.size(); ++i) {
        if (d->m_vector.at(i) == object) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief add an element
 */
void GeoDataMultiGeometry::append(GeoDataGeometry *other)
{
    detach();

    Q_D(GeoDataMultiGeometry);
    other->setParent(this);
    d->m_vector.append(other);
}

GeoDataMultiGeometry &GeoDataMultiGeometry::operator<<(const GeoDataGeometry &value)
{
    detach();

    Q_D(GeoDataMultiGeometry);
    GeoDataGeometry *g = value.copy();
    g->setParent(this);
    d->m_vector.append(g);
    return *this;
}

void GeoDataMultiGeometry::clear()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    qDeleteAll(d->m_vector);
    d->m_vector.clear();
}

void GeoDataMultiGeometry::pack(QDataStream &stream) const
{
    Q_D(const GeoDataMultiGeometry);

    GeoDataGeometry::pack(stream);

    stream << d->m_vector.size();

    for (QList<GeoDataGeometry *>::const_iterator iterator = d->m_vector.constBegin(); iterator != d->m_vector.constEnd(); ++iterator) {
        const GeoDataGeometry *geometry = *iterator;
        stream << geometry->geometryId();
        geometry->pack(stream);
    }
}

void GeoDataMultiGeometry::unpack(QDataStream &stream)
{
    detach();

    Q_D(GeoDataMultiGeometry);
    GeoDataGeometry::unpack(stream);

    int size = 0;

    stream >> size;

    for (int i = 0; i < size; i++) {
        int geometryId;
        stream >> geometryId;
        switch (geometryId) {
        case InvalidGeometryId:
            break;
        case GeoDataPointId: {
            auto point = new GeoDataPoint;
            point->unpack(stream);
            d->m_vector.append(point);
        } break;
        case GeoDataLineStringId: {
            auto lineString = new GeoDataLineString;
            lineString->unpack(stream);
            d->m_vector.append(lineString);
        } break;
        case GeoDataLinearRingId: {
            auto linearRing = new GeoDataLinearRing;
            linearRing->unpack(stream);
            d->m_vector.append(linearRing);
        } break;
        case GeoDataPolygonId: {
            auto polygon = new GeoDataPolygon;
            polygon->unpack(stream);
            d->m_vector.append(polygon);
        } break;
        case GeoDataMultiGeometryId: {
            auto multiGeometry = new GeoDataMultiGeometry;
            multiGeometry->unpack(stream);
            d->m_vector.append(multiGeometry);
        } break;
        case GeoDataModelId:
            break;
        default:
            break;
        };
    }
}

}
