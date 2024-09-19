// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataBuilding.h"
#include "GeoDataBuilding_p.h"
#include "GeoDataTypes.h"

namespace Marble {

GeoDataBuilding::GeoDataBuilding()
    : GeoDataGeometry(new GeoDataBuildingPrivate),
      d(new GeoDataBuildingPrivate)
{
}

GeoDataBuilding::GeoDataBuilding(const GeoDataGeometry &other)
    : GeoDataGeometry(other),
      d(new GeoDataBuildingPrivate)
{
}

GeoDataBuilding::GeoDataBuilding(const GeoDataBuilding &other)
    : GeoDataGeometry(other),
      d(new GeoDataBuildingPrivate(*other.d))
{
}

GeoDataBuilding::~GeoDataBuilding()
{
    delete d;
}

GeoDataBuilding& GeoDataBuilding::operator=(const GeoDataBuilding& other)
{
    GeoDataGeometry::operator=(other);
    *d = *other.d;
    return *this;
}

const char *GeoDataBuilding::nodeType() const
{
    return GeoDataTypes::GeoDataBuildingType;
}

EnumGeometryId GeoDataBuilding::geometryId() const
{
    return GeoDataBuildingId;
}

GeoDataGeometry *GeoDataBuilding::copy() const
{
    return new GeoDataBuilding(*this);
}

double GeoDataBuilding::height() const
{
    return d->m_height;
}

void GeoDataBuilding::setHeight(double height)
{
    d->m_height = height;
}

int GeoDataBuilding::minLevel() const
{
    return d->m_minLevel;
}

void GeoDataBuilding::setMinLevel(int minLevel)
{
    d->m_minLevel = minLevel;
}

int GeoDataBuilding::maxLevel() const
{
    return d->m_maxLevel;
}

void GeoDataBuilding::setMaxLevel(int maxLevel)
{
    d->m_maxLevel = maxLevel;
}

QVector<int> GeoDataBuilding::nonExistentLevels() const
{
    return d->m_nonExistentLevels;
}

void GeoDataBuilding::setNonExistentLevels(const QVector<int> &nonExistentLevels)
{
    d->m_nonExistentLevels = nonExistentLevels;
}

GeoDataMultiGeometry* GeoDataBuilding::multiGeometry() const
{
    return &d->m_multiGeometry;
}

const GeoDataLatLonAltBox &GeoDataBuilding::latLonAltBox() const
{
    // @TODO: This is temporary, for only when we have just one child
    Q_ASSERT(d->m_multiGeometry.size() == 1);
    return static_cast<const GeoDataMultiGeometry>(d->m_multiGeometry).at(0).latLonAltBox();
}

QString GeoDataBuilding::name() const
{
    return d->m_name;
}

void GeoDataBuilding::setName(const QString& name)
{
    d->m_name = name;
}

QVector<GeoDataBuilding::NamedEntry> GeoDataBuilding::entries() const
{
    return d->m_entries;
}

void GeoDataBuilding::setEntries(const QVector<GeoDataBuilding::NamedEntry> &entries)
{
    d->m_entries = entries;
}

double GeoDataBuilding::parseBuildingHeight(const QString& buildingHeight)
{
    double height = 8.0;

    // check first for unitless value
    bool converted;
    double extractedHeight = buildingHeight.toDouble(&converted);
    if (converted) {
        return extractedHeight;
    }

    if (buildingHeight.endsWith(QLatin1Char('m')) ||
        buildingHeight.endsWith(QLatin1String("meter")) ||
        buildingHeight.endsWith(QLatin1String("meters")) ||
        buildingHeight.endsWith(QLatin1String("metre")) ||
        buildingHeight.endsWith(QLatin1String("metres"))) {
        QString const heightValue = QString(buildingHeight).remove(QStringLiteral("meters"))
                .remove(QStringLiteral("meter")).remove(QStringLiteral("metres"))
                .remove(QStringLiteral("metre")).remove(QLatin1Char('m')).trimmed();
        bool extracted;
        double extractedHeight = heightValue.toDouble(&extracted);
        if (extracted) {
            height = extractedHeight;
        }
    } else { // feet and inches
        double extractedHeight = 0.0; // in inches, converted to meters in the end
        if (buildingHeight.contains(QLatin1Char('\''))) {
            double heightInches = 0.0;
            QStringList const feetInches = buildingHeight.split(QLatin1Char('\''));
            bool okFeet;
            double feet = feetInches[0].trimmed().toDouble(&okFeet);
            if (okFeet) {
                heightInches = feet * FT2IN;
            }
            if (!feetInches[1].isEmpty()) { // has inches as unit as well
                bool okInches;
                double inches = QString(feetInches[1]).remove(QLatin1Char('\"')).trimmed().toDouble(&okInches);
                if (okInches) {
                    heightInches += inches;
                }
            }
            extractedHeight = heightInches;
        } else if (buildingHeight.endsWith(QLatin1String("feet"))) {
            bool ok;
            double feet = QString(buildingHeight).remove(QStringLiteral("feet")).trimmed().toDouble(&ok);
            if (ok) {
                extractedHeight = feet * FT2IN;
            }
        }
        if (extractedHeight > 0.0) {
            height = extractedHeight * IN2M; // convert inches to meters
        }
    }

    return height;
}

}
