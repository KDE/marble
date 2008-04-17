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

#ifndef GEOSCENELAYER_H
#define GEOSCENELAYER_H

#include <QtCore/QVector>

#include "GeoDocument.h"
#include "GeoSceneItem.h"


/**
 * @short Contents used inside a layer.
 */
class GeoSceneAbstractDataset : public GeoNode {
 public:
    virtual ~GeoSceneAbstractDataset() {};

    QString name() const;

    QString fileFormat() const;
    void setFileFormat( const QString& fileFormat );

    int expire() const;
    void setExpire( int expire );

    virtual QString type() = 0;

 protected:
    GeoSceneAbstractDataset( const QString& name );

    QString m_name;
    QString m_fileFormat;
    int     m_expire;
};

/**
 * @short Layer of a GeoScene document.
 */

class GeoSceneLayer : public GeoNode {
 public:
    GeoSceneLayer( const QString& name );
    ~GeoSceneLayer();

    /**
     * @brief  Add a section to the legend
     * @param  section  the new section
     */
    void addDataset( GeoSceneAbstractDataset* );
    GeoSceneAbstractDataset* dataset( const QString& );
    QVector<GeoSceneAbstractDataset*> datasets() const;

    QString name() const;

    QString backend() const;
    void setBackend( const QString& plugin );

    QString role() const;
    void setRole( const QString& type );

 protected:
    /// The vector holding all the data in the layer.
    /// (We want to preserve the order and don't care 
    /// much about speed here), so we don't use a hash
    QVector<GeoSceneAbstractDataset*> m_datasets;

    QString m_name;
    QString m_backend;
    QString m_role;
};

#endif // GEOSCENELAYER_H
