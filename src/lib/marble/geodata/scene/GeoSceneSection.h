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

#ifndef MARBLE_GEOSCENESECTION_H
#define MARBLE_GEOSCENESECTION_H

#include <QString>
#include <QVector>

#include "GeoDocument.h"

namespace Marble
{

class GeoSceneItem;

/**
 * @short Section of a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneSection : public GeoNode
{
 public:
    explicit GeoSceneSection( const QString& name );
    ~GeoSceneSection() override;

    const char* nodeType() const override;

    /**
     * @brief  Add an item to the legend section
     * @param  item  the new item
     */
    void addItem(GeoSceneItem*);
    GeoSceneItem* item(const QString&);
    QVector<GeoSceneItem*> items() const;

    QString name() const;

    QString heading() const;
    void setHeading( const QString& heading );

    bool checkable() const;
    void setCheckable( bool checkable );

    QString connectTo() const;
    void setConnectTo( const QString& text );

    int  spacing() const;
    void setSpacing( int spacing );

    QString radio() const;
    void setRadio( const QString& radio );

 private:
    Q_DISABLE_COPY( GeoSceneSection )

    /// The vector holding all the items in the legend section.
    /// (We want to preserve the order and don't care
    /// much about speed here), so we don't use a hash
    QVector<GeoSceneItem*> m_items;

    QString m_name;
    QString m_heading;
    QString m_connectTo;
    QString m_radio;

    bool    m_checkable;
    int     m_spacing;
};

}

#endif
