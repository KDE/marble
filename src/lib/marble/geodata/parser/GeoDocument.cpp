/*
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoDocument.h"
#include <cstdio>
#include "GeoSceneTypes.h"

namespace Marble
{

GeoDocument::GeoDocument()
{
}

GeoDocument::~GeoDocument()
{
}

bool GeoDocument::isGeoDataDocument() const
{
    return false;
}

bool GeoDocument::isGeoSceneDocument() const
{
    return false;
}


GeoNode::GeoNode()
{
}

GeoNode::~GeoNode()
{
}

}
