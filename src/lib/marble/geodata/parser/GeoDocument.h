/*
    Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MARBLE_GEODOCUMENT_H
#define MARBLE_GEODOCUMENT_H

#include "geodata_export.h"

namespace Marble
{

/**
 * @short A shared base class between GeoDataDocument/GeoSourceDocument
 */
class GEODATA_EXPORT GeoDocument
{
protected:
    GeoDocument();

public:
    virtual ~GeoDocument();

    virtual bool isGeoDataDocument() const;
    virtual bool isGeoSceneDocument() const;
};

/**
 * @short A shared base class for all classes that are mapped to a specific tag (ie. GeoFolder)
 */
class GEODATA_EXPORT GeoNode
{
public:
    GeoNode();
    virtual ~GeoNode();

    /// Provides type information for downcasting a GeoNode
    virtual const char* nodeType() const = 0;
};

}

#endif
