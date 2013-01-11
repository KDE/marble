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

#ifndef MARBLE_GEOSCENEVECTOR_H
#define MARBLE_GEOSCENEVECTOR_H

#include <QtCore/QString>
#include <QtGui/QPen>
#include <QtGui/QBrush>

#include "GeoSceneLayer.h"

namespace Marble
{

/**
 * @short Vector dataset stored in a layer.
 */

class GEODATA_EXPORT GeoSceneVector : public GeoSceneAbstractDataset
{
 public:
    explicit GeoSceneVector( const QString& name );
    virtual const char* nodeType() const;

    QString sourceFile() const;
    void setSourceFile( const QString& sourceFile );

    QString feature() const;
    void setFeature( const QString& feature );

    QPen pen() const;
    void setPen( const QPen& pen );

    QBrush brush() const;
    void setBrush( const QBrush& brush );

    virtual QString type();

 private:
    QString m_sourceFile;
    QString m_feature;

    QPen    m_pen;
    QBrush  m_brush;
};

}

#endif
