//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

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

void GpxParser::raiseRootElementError()
{
    raiseError(QObject::tr("The file is not a valid GPX 1.0 / 1.1 file"));
}

bool GpxParser::isValidElement(const QString& tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    return (   namespaceUri() == gpx::gpxTag_nameSpace10
            || namespaceUri() == gpx::gpxTag_nameSpace11);
}

GeoDocument* GpxParser::createDocument() const
{
    return new GeoDataDocument;
}

}
