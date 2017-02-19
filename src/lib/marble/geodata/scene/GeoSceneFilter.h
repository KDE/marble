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

#ifndef MARBLE_GEOSCENEFILTER_H
#define MARBLE_GEOSCENEFILTER_H

#include <QString>
#include <QList>

#include "GeoDocument.h"

namespace Marble
{

class GeoScenePalette;

/**
 * @short Filter of a GeoScene document.
 */

class GeoSceneFilter : public GeoNode
{
 public:
    explicit GeoSceneFilter( const QString& name );
    ~GeoSceneFilter() override;

    QString name() const;
    void setName( const QString& name );

    QString type() const;
    void setType( const QString& type );

    QList<const GeoScenePalette*> palette() const;
    void addPalette( const GeoScenePalette *palette );
    int removePalette( const GeoScenePalette *palette ); //returns # of items removed

    const char *nodeType() const override;

 private:
    QString m_name;
    QString m_type;
    QList<const GeoScenePalette*> m_palette;

};

}

#endif
