/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2007 Murad Tagirov <tmurad@gmail.com>

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

#ifndef GEOSCENEDOCUMENT_H
#define GEOSCENEDOCUMENT_H

#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoSceneSettings.h"
#include "GeoSceneLegend.h"
#include "GeoDocument.h"

/**
 * @short A container for features parsed from the DGML file.
 */
class GeoSceneDocument : public GeoDocument,
                         public GeoNode {
 public:
    GeoSceneDocument();
    ~GeoSceneDocument();

    virtual bool isGeoSceneDocument() const { return true; }

    GeoSceneHead*     head() const;
    GeoSceneMap*      map() const;
    GeoSceneSettings* settings() const;
    GeoSceneLegend*   legend() const;

 protected:
    GeoSceneHead*     m_head;
    GeoSceneMap*      m_map;
    GeoSceneSettings* m_settings;
    GeoSceneLegend*   m_legend;
};


#endif // GEOSCENEDOCUMENT_H
