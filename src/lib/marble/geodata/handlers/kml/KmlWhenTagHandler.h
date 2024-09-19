// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef KmlWhenTagHandler_h
#define KmlWhenTagHandler_h

#include "GeoTagHandler.h"
#include <GeoDataTimeStamp.h>

class QString;

namespace Marble
{
namespace kml
{

class KmlwhenTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;

    static QDateTime parse(const QString &dateTime);

    static GeoDataTimeStamp parseTimestamp(const QString &dateTime);

private:
    static Marble::GeoDataTimeStamp::TimeResolution modify(QString &whenString);
};

}
}

#endif // KmlWhenTagHandler_h
