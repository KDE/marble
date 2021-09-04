// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "GpxParser.h"
#include "GPXElementDictionary.h"
#include "GeoDataDocument.h"

namespace Marble {

GpxParser::GpxParser()
    : GeoParser( 0 )
{
}

GpxParser::~GpxParser()
{
}

bool GpxParser::isValidRootElement()
{
    return isValidElement(gpx::gpxTag_gpx);
}

bool GpxParser::isValidElement(const QString& tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    return (   namespaceUri() == gpx::gpxTag_nameSpace10
            || namespaceUri() == gpx::gpxTag_nameSpace11
            || namespaceUri() == gpx::gpxTag_nameSpaceGarminTrackPointExt1);
}

GeoDocument* GpxParser::createDocument() const
{
    return new GeoDataDocument;
}

}
