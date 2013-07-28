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

#ifndef MARBLE_GEOSCENELEGEND_H
#define MARBLE_GEOSCENELEGEND_H

#include <QVector>
#include "GeoDocument.h"

#include <geodata_export.h>

namespace Marble
{

class GeoSceneSection;

class GeoSceneLegendPrivate;

/**
 * @short Legend of a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneLegend : public GeoNode
{
 public:
    GeoSceneLegend();
    ~GeoSceneLegend();
    
    virtual const char* nodeType() const;

    /**
     * @brief  Add a section to the legend
     * @param  section  the new section
     */
    void addSection( const GeoSceneSection* );
    QVector<const GeoSceneSection*> sections() const;

 private:
    Q_DISABLE_COPY( GeoSceneLegend )
    GeoSceneLegendPrivate * const d;
};

}

#endif
