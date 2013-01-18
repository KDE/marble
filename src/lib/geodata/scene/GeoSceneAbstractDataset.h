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

#ifndef MARBLE_GEOSCENEABSTRACTDATASET_H
#define MARBLE_GEOSCENEABSTRACTDATASET_H

#include <QtCore/QString>

#include <geodata_export.h>

#include "GeoDocument.h"

namespace Marble
{

/**
 * @short Contents used inside a layer.
 */
class GEODATA_EXPORT GeoSceneAbstractDataset : public GeoNode
{
 public:
    virtual ~GeoSceneAbstractDataset() {};

    QString name() const;

    QString fileFormat() const;
    void setFileFormat( const QString& fileFormat );

    int expire() const;
    void setExpire( int expire );

    virtual QString type() = 0;

 protected:
    explicit GeoSceneAbstractDataset( const QString& name );

 private:
    QString m_name;
    QString m_fileFormat;
    int     m_expire;
};

}

#endif
