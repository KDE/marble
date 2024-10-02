// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
// SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
//

#include "GeoDataGeometry.h"
#include "GeoDataGeometry_p.h"

#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataModel.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"

#include "MarbleDebug.h"

#include <QDataStream>

namespace Marble
{

GeoDataGeometry::GeoDataGeometry(const GeoDataGeometry &other)
    : GeoDataObject()
    , d_ptr(other.d_ptr)
{
    d_ptr->ref.ref();
}

GeoDataGeometry::GeoDataGeometry(GeoDataGeometryPrivate *priv)
    : GeoDataObject()
    , d_ptr(priv)
{
    d_ptr->ref.ref();
}

GeoDataGeometry::~GeoDataGeometry()
{
    if (!d_ptr->ref.deref())
        delete d_ptr;
}

void GeoDataGeometry::detach()
{
    if (d_ptr->ref.fetchAndAddRelaxed(0) == 1) {
        return;
    }

    GeoDataGeometryPrivate *new_d = d_ptr->copy();

    if (!d_ptr->ref.deref())
        delete d_ptr;

    d_ptr = new_d;
    d_ptr->ref.ref();
}

GeoDataGeometry &GeoDataGeometry::operator=(const GeoDataGeometry &other)
{
    GeoDataObject::operator=(other);

    if (!d_ptr->ref.deref())
        delete d_ptr;

    d_ptr = other.d_ptr;
    d_ptr->ref.ref();

    return *this;
}

bool GeoDataGeometry::operator==(const GeoDataGeometry &other) const
{
    if (nodeType() != other.nodeType()) {
        return false;
    }

    if (nodeType() == GeoDataTypes::GeoDataPolygonType) {
        const auto &thisPoly = static_cast<const GeoDataPolygon &>(*this);
        const auto &otherPoly = static_cast<const GeoDataPolygon &>(other);

        return thisPoly == otherPoly;
    } else if (nodeType() == GeoDataTypes::GeoDataLinearRingType) {
        const auto &thisRing = static_cast<const GeoDataLinearRing &>(*this);
        const auto &otherRing = static_cast<const GeoDataLinearRing &>(other);

        return thisRing == otherRing;
    } else if (nodeType() == GeoDataTypes::GeoDataLineStringType) {
        const auto &thisLine = static_cast<const GeoDataLineString &>(*this);
        const auto &otherLine = static_cast<const GeoDataLineString &>(other);

        return thisLine == otherLine;
    } else if (nodeType() == GeoDataTypes::GeoDataModelType) {
        const auto &thisModel = static_cast<const GeoDataModel &>(*this);
        const auto &otherModel = static_cast<const GeoDataModel &>(other);

        return thisModel == otherModel;
    } else if (nodeType() == GeoDataTypes::GeoDataMultiGeometryType) {
        const auto &thisMG = static_cast<const GeoDataMultiGeometry &>(*this);
        const auto &otherMG = static_cast<const GeoDataMultiGeometry &>(other);

        return thisMG == otherMG;
    } else if (nodeType() == GeoDataTypes::GeoDataTrackType) {
        const auto &thisTrack = static_cast<const GeoDataTrack &>(*this);
        const auto &otherTrack = static_cast<const GeoDataTrack &>(other);

        return thisTrack == otherTrack;
    } else if (nodeType() == GeoDataTypes::GeoDataMultiTrackType) {
        const auto &thisMT = static_cast<const GeoDataMultiTrack &>(*this);
        const auto &otherMT = static_cast<const GeoDataMultiTrack &>(other);

        return thisMT == otherMT;
    } else if (nodeType() == GeoDataTypes::GeoDataPointType) {
        const auto &thisPoint = static_cast<const GeoDataPoint &>(*this);
        const auto &otherPoint = static_cast<const GeoDataPoint &>(other);

        return thisPoint == otherPoint;
    }

    return false;
}

bool GeoDataGeometry::extrude() const
{
    return d_ptr->m_extrude;
}

void GeoDataGeometry::setExtrude(bool extrude)
{
    detach();
    d_ptr->m_extrude = extrude;
}

AltitudeMode GeoDataGeometry::altitudeMode() const
{
    return d_ptr->m_altitudeMode;
}

void GeoDataGeometry::setAltitudeMode(const AltitudeMode altitudeMode)
{
    detach();
    d_ptr->m_altitudeMode = altitudeMode;
}

const GeoDataLatLonAltBox &GeoDataGeometry::latLonAltBox() const
{
    return d_ptr->m_latLonAltBox;
}

void GeoDataGeometry::pack(QDataStream &stream) const
{
    GeoDataObject::pack(stream);

    stream << d_ptr->m_extrude;
    stream << d_ptr->m_altitudeMode;
}

void GeoDataGeometry::unpack(QDataStream &stream)
{
    detach();
    GeoDataObject::unpack(stream);

    int am;
    stream >> d_ptr->m_extrude;
    stream >> am;
    d_ptr->m_altitudeMode = (AltitudeMode)am;
}

bool GeoDataGeometry::equals(const GeoDataGeometry &other) const
{
    return GeoDataObject::equals(other) && d_ptr->m_extrude == other.d_ptr->m_extrude && d_ptr->m_altitudeMode == other.d_ptr->m_altitudeMode;
}

}
