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

#ifndef GEOSCENELEGEND_H
#define GEOSCENELEGEND_H

#include <QtCore/QVector>

#include "GeoDocument.h"

class GeoSceneSection;

/**
 * @short Legend of a GeoScene document.
 */

class GeoSceneLegend : public GeoNode {
 public:
    GeoSceneLegend();
    ~GeoSceneLegend();

    /**
     * @brief  Add a section to the legend
     * @param  section  the new section
     */
    void addSection( GeoSceneSection* );
    GeoSceneSection* section( const QString& );
    QVector<GeoSceneSection*> sections() const;

 protected:
    /// The vector holding all the sections in the legend.
    /// (We want to preserve the order and don't care 
    /// much about speed here), so we don't use a hash
    QVector<GeoSceneSection*> m_sections;
};


#endif // GEOSCENELEGEND_H
