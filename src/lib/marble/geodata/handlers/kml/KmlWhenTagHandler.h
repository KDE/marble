//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
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
    virtual GeoNode* parse(GeoParser&) const;

    static QDateTime parse( const QString &dateTime );

    static GeoDataTimeStamp parseTimestamp( const QString &dateTime );

private:
    static Marble::GeoDataTimeStamp::TimeResolution modify( QString& whenString );
};

}
}

#endif // KmlWhenTagHandler_h
