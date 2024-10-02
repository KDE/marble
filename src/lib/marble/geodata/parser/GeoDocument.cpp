/*
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoDocument.h"
#include <cstdio>

namespace Marble
{

GeoDocument::GeoDocument() = default;

GeoDocument::~GeoDocument() = default;

bool GeoDocument::isGeoDataDocument() const
{
    return false;
}

bool GeoDocument::isGeoSceneDocument() const
{
    return false;
}

GeoNode::GeoNode() = default;

GeoNode::~GeoNode() = default;
}
