// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATAMODEL_H
#define GEODATAMODEL_H

#include "GeoDataGeometry.h"
#include "GeoDataLink.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataCoordinates;
class GeoDataLink;
class GeoDataLocation;
class GeoDataOrientation;
class GeoDataResourceMap;
class GeoDataScale;
class GeoDataModelPrivate;

/**
 */
class GEODATA_EXPORT GeoDataModel : public GeoDataGeometry
{
public:
    GeoDataModel();

    GeoDataModel(const GeoDataModel &other);

    GeoDataModel &operator=(const GeoDataModel &other);

    const char *nodeType() const override;

    EnumGeometryId geometryId() const override;

    GeoDataGeometry *copy() const override;

    bool operator==(const GeoDataModel &other) const;
    bool operator!=(const GeoDataModel &other) const;

    ~GeoDataModel() override;

    const GeoDataLink &link() const;
    GeoDataLink &link();
    void setLink(const GeoDataLink &link);

    const GeoDataCoordinates &coordinates() const;
    GeoDataCoordinates &coordinates();
    void setCoordinates(const GeoDataCoordinates &coordinates);

    const GeoDataScale &scale() const;
    GeoDataScale &scale();
    void setScale(const GeoDataScale &scale);

    const GeoDataOrientation &orientation() const;
    GeoDataOrientation &orientation();
    void setOrientation(const GeoDataOrientation &orientation);

    const GeoDataLocation &location() const;
    GeoDataLocation &location();
    void setLocation(const GeoDataLocation &location);

    const GeoDataResourceMap &resourceMap() const;
    GeoDataResourceMap &resourceMap();
    void setResourceMap(const GeoDataResourceMap &map);

    QString sourceHref() const;
    QString targetHref() const;
    void setSourceHref(const QString &sourceHref);
    void setTargetHref(const QString &targetHref);

private:
    Q_DECLARE_PRIVATE(GeoDataModel)
};

}

#endif // GEODATAMODEL_H
