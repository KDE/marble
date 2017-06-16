//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef MARBLE_GEODATABUILDING_H
#define MARBLE_GEODATABUILDING_H

#include <QVector>

#include "GeoDataGeometry.h"

#include "geodata_export.h"

namespace Marble {
class GeoDataBuildingPrivate;

/*!
    \class GeoDataBuilding
    \brief Contains important information about a building and its floors (levels)

    GeoDataBuilding holds information such as minimum floor, maximum floor,
    floor data and their respective MultiGeometry and other possible metadata such
    as the total height of the building, type etc.
 */

class GEODATA_EXPORT GeoDataBuilding : public GeoDataGeometry
{
public:
    explicit GeoDataBuilding();
    explicit GeoDataBuilding(const GeoDataGeometry &other);
    explicit GeoDataBuilding(const GeoDataBuilding &other);

    GeoDataBuilding& operator=(const GeoDataBuilding &other);

    const char *nodeType() const override;

    EnumGeometryId geometryId() const override;

    GeoDataGeometry *copy() const override;

    /*!
    Destroys the GeoDataBuilding
*/
    ~GeoDataBuilding() override;


/*!
    @return the height of the building
*/
    double height() const;


/*!
    Sets the height of the building
    @param height
 */
    void setHeight(double height);


/*!
    @return the minimum level
 */
    int minLevel() const;


/*!
    Sets the minimum level of the building
    @param minLevel
 */
    void setMinLevel(int minLevel);


/*!
    @return the maximum level of the building
 */
    int maxLevel() const;


/*!
    Sets the maximum level of the building
    @param maxLevel
 */
    void setMaxLevel(int maxLevel);


/*!
    @return the non existent levels in the building
 */
    QVector<int> nonExistentLevels() const;


/*!
    Sets the non existent levels of the building
    @param nonExistentLevels
 */
    void setNonExistentLevels(const QVector<int>& nonExistentLevels);


/*!
 * @return the multigeometry associated with the building
 */
    GeoDataMultiGeometry* multiGeometry() const;

private:
    GeoDataBuildingPrivate* const d;
};

}

#endif
