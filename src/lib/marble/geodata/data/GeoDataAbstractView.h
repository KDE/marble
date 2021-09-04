// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#ifndef MARBLE_GEODATAABSTRACTVIEW_H
#define MARBLE_GEODATAABSTRACTVIEW_H

#include "MarbleGlobal.h"
#include "GeoDataObject.h"

#include "geodata_export.h"

namespace Marble
{
class GeoDataCoordinates;
class GeoDataTimeSpan;
class GeoDataTimeStamp;
class GeoDataAbstractViewPrivate;

/**
 * @see GeoDataLookAt
 */
class GEODATA_EXPORT GeoDataAbstractView : public GeoDataObject
{
 public:
    GeoDataAbstractView();

    ~GeoDataAbstractView() override;

    GeoDataAbstractView( const GeoDataAbstractView &other );

    GeoDataAbstractView& operator=( const GeoDataAbstractView &other );

    bool operator==(const GeoDataAbstractView &other) const;
    inline bool operator!=(const GeoDataAbstractView &other) const { return !(*this == other); }

    virtual GeoDataAbstractView *copy() const = 0;

    const GeoDataTimeSpan& timeSpan() const;

    GeoDataTimeSpan& timeSpan();

    void setTimeSpan( const GeoDataTimeSpan &timeSpan );

    GeoDataTimeStamp& timeStamp();

    const GeoDataTimeStamp& timeStamp() const;

    void setTimeStamp( const GeoDataTimeStamp &timeStamp );

    AltitudeMode altitudeMode() const;

    void setAltitudeMode(const AltitudeMode altitudeMode);

    GeoDataCoordinates coordinates() const;

private:
    GeoDataAbstractViewPrivate* const d;

protected:
    bool equals(const GeoDataAbstractView &other) const;

    using GeoDataObject::equals;
};

} // namespace Marble

#endif
