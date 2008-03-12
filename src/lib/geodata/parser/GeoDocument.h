/*
    Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>

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

#ifndef GeoDocument_h
#define GeoDocument_h

#include <stdio.h>

#include "GeoTagHandler.h"

// Set to a value greater than 0, to enable leak tracking of GeoNode objects
// Set to a value greater than 1, to enable detailed tracking of construction/destruction of GeoNode objects
#define DUMP_GEONODE_LEAKS 1

/**
 * @short A shared base class between GeoDataDocument/GeoSourceDocument
 */
class GeoDocument {
protected:
    GeoDocument() { }

public:
#if DUMP_GEONODE_LEAKS > 0
    static unsigned long s_leakProtector;
#endif

    virtual ~GeoDocument()
    {
#if DUMP_GEONODE_LEAKS > 0
        if (s_leakProtector != 0) {
            fprintf(stderr, "Found %li GeoNode object LEAKS!\n", s_leakProtector);
            s_leakProtector = 0;
        }
#endif
    }

    virtual bool isGeoDataDocument() const { return false; }
    virtual bool isGeoSceneDocument() const { return false; }
};

/**
 * @short A shared base class for all classes that are mapped to a specifig tag (ie. GeoFolder)
 */
class GeoNode {
protected:
#if DUMP_GEONODE_LEAKS > 0
    GeoNode()
    {
        GeoDocument::s_leakProtector++;

#if DUMP_GEONODE_LEAKS > 1
        fprintf(stderr, "Constructed new GeoNode object, leak protection count: %li\n", GeoDocument::s_leakProtector);
#endif
    }

    virtual ~GeoNode()
    {
        --GeoDocument::s_leakProtector;

#if DUMP_GEONODE_LEAKS > 1
        fprintf(stderr, "Destructed GeoNode object, leak protection 1count: %li\n", GeoDocument::s_leakProtector);
#endif
    }
#else
    GeoNode() { }
    virtual ~GeoNode() { }
#endif
};

#endif // GeoDocument_h
