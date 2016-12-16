//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees  <nafees.technocool@gmail.com>
//

#ifndef GEODATAIMAGEPYRAMID_H
#define GEODATAIMAGEPYRAMID_H

#include "GeoDataObject.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataImagePyramidPrivate;

/**
 */
class GEODATA_EXPORT GeoDataImagePyramid : public GeoDataObject
{
public:
    GeoDataImagePyramid();

    GeoDataImagePyramid( const GeoDataImagePyramid &other );

    GeoDataImagePyramid& operator=( const GeoDataImagePyramid &other );
    bool operator==( const GeoDataImagePyramid &other ) const;
    bool operator!=( const GeoDataImagePyramid &other ) const;

    ~GeoDataImagePyramid() override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    enum GridOrigin {
        LowerLeft,
        UpperLeft
    };

    int tileSize() const;
    void setTileSize(int tileSize);

    int maxWidth() const;
    void setMaxWidth(int maxWidth);

    int maxHeight() const;
    void setMaxHeight(int maxHeight);

    GridOrigin gridOrigin() const;
    void setGridOrigin(GridOrigin gridOrigin);

private:
    GeoDataImagePyramidPrivate* const d;
};

}

#endif
