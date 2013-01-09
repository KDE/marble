//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "OsmParser.h"
#include "OsmElementDictionary.h"
#include "GeoDataDocument.h"

#include "OsmNodeFactory.h"
#include "OsmWayFactory.h"
#include "OsmRelationFactory.h"
#include "OsmGlobals.h"

namespace Marble {

OsmParser::OsmParser()
    : GeoParser( 0 )
{
}

OsmParser::~OsmParser()
{
    osm::OsmRelationFactory::clear();
    osm::OsmWayFactory::clear();
    osm::OsmNodeFactory::clear();
    osm::OsmGlobals::cleanUpDummyPlacemarks();
}

bool OsmParser::isValidRootElement()
{
    return isValidElement(osm::osmTag_osm);
}

bool OsmParser::isValidElement(const QString& tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    //always "valid" because there is no namespace
    return true;
}

GeoDocument* OsmParser::createDocument() const
{
    return new GeoDataDocument;
}

}
