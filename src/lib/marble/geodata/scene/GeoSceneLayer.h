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

#ifndef MARBLE_GEOSCENELAYER_H
#define MARBLE_GEOSCENELAYER_H

#include <QString>
#include <QVector>

#include <geodata_export.h>

#include "GeoDocument.h"

namespace Marble
{

class GeoSceneAbstractDataset;
class GeoSceneFilter;
class GeoSceneLayerPrivate;

/**
 * @short Layer of a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneLayer : public GeoNode
{
 public:
    explicit GeoSceneLayer( const QString& name );
    ~GeoSceneLayer() override;
    
    const char* nodeType() const override;

    /**
     * @brief  Add a data set to the legend
     * @param  dataset  the new data set
     */
    void addDataset( GeoSceneAbstractDataset* );

    const GeoSceneAbstractDataset * dataset( const QString& ) const;
    GeoSceneAbstractDataset * dataset( const QString& );

    const GeoSceneAbstractDataset * groundDataset() const;
    GeoSceneAbstractDataset * groundDataset();

    QVector<GeoSceneAbstractDataset*> datasets() const;

    QString name() const;

    QString backend() const;
    void setBackend( const QString& plugin );

    /**
     * @brief  returns whether the data is organized in quad tiles.
     */
    bool isTiled() const;
    void setTiled( bool );

    QString role() const;
    void setRole( const QString& type );

    const GeoSceneFilter * filter() const;
    GeoSceneFilter * filter();
    void addFilter( GeoSceneFilter * filter );
    void removeFilter( GeoSceneFilter * filter );

 private:
    Q_DISABLE_COPY( GeoSceneLayer )

    GeoSceneLayerPrivate * const d;
};

}

#endif
