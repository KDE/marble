/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

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

#ifndef GEOSCENEXMLDATASOURCE_H
#define GEOSCENEXMLDATASOURCE_H

#include <QtCore/QUrl>
#include <QtCore/QVector>

#include <geodata_export.h>

#include "GeoSceneLayer.h"

/**
 * @short Texture dataset stored in a layer.
 */

namespace Marble
{

class GEODATA_EXPORT GeoSceneXmlDataSource : public GeoSceneAbstractDataset
{
 public:
    explicit GeoSceneXmlDataSource( const QString& name );
    ~GeoSceneXmlDataSource();

    QString filename() const;
    void setFilename( const QString& fileName );

    virtual QString type();
 private:
    QString m_filename;
};

}

#endif // GEOSCENEXMLDATASOURCE_H
