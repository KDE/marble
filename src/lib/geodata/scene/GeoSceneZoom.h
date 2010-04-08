/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

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

#ifndef MARBLE_GEOSCENEZOOM_H
#define MARBLE_GEOSCENEZOOM_H

#include <geodata_export.h>
#include "GeoDocument.h"

namespace Marble
{

/**
 * @short Zoom properties of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneZoom : public GeoNode
{
 public:
    GeoSceneZoom();

    int minimum() const;
    void setMinimum( int name );
    int maximum() const;
    void setMaximum( int target );
    bool discrete() const;
    void setDiscrete( bool theme );

 private:
    // FIXME: d-pointerfy

    int  m_minimum;
    int  m_maximum;
    bool m_discrete;
};

}

#endif
