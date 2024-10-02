// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATACONTAINERPRIVATE_H
#define MARBLE_GEODATACONTAINERPRIVATE_H

#include "GeoDataFeature_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataContainerPrivate : public GeoDataFeaturePrivate
{
public:
    GeoDataContainerPrivate() = default;

    GeoDataContainerPrivate(const GeoDataContainerPrivate &other)
        : GeoDataFeaturePrivate(other)
    {
        m_vector.reserve(other.m_vector.size());
        for (GeoDataFeature *feature : other.m_vector) {
            m_vector.append(feature->clone());
        }
    }

    ~GeoDataContainerPrivate() override
    {
        qDeleteAll(m_vector);
    }

    GeoDataContainerPrivate &operator=(const GeoDataContainerPrivate &other)
    {
        GeoDataFeaturePrivate::operator=(other);
        qDeleteAll(m_vector);
        m_vector.clear();
        m_vector.reserve(other.m_vector.size());
        for (GeoDataFeature *feature : other.m_vector) {
            m_vector.append(feature->clone());
        }
        return *this;
    }

    EnumFeatureId featureId() const override
    {
        return GeoDataFolderId;
    }

    void setParent(GeoDataObject *parent)
    {
        for (GeoDataFeature *feature : std::as_const(m_vector)) {
            feature->setParent(parent);
        }
    }

    QList<GeoDataFeature *> m_vector;
};

} // namespace Marble

#endif
