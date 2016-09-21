//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Gaurav Gupta <1989.gaurav@googlemail.com>
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

    ~GeoDataAbstractView();

    GeoDataAbstractView( const GeoDataAbstractView &other );

    GeoDataAbstractView& operator=( const GeoDataAbstractView &other );

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
