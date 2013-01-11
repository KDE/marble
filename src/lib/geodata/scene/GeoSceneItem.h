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

#ifndef MARBLE_GEOSCENEITEM_H
#define MARBLE_GEOSCENEITEM_H

#include <QtCore/QString>

#include "GeoDocument.h"

namespace Marble
{

class GeoSceneIcon;

/**
 * @short The section item in a legend of a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneItem : public GeoNode
{
 public:
    explicit GeoSceneItem( const QString& name );
    ~GeoSceneItem();
    
    virtual const char* nodeType() const;

    QString name() const;

    QString text() const;
    void setText( const QString& text );

    bool checkable() const;
    void setCheckable( bool checkable );

    QString connectTo() const;
    void setConnectTo( const QString& text );

    int  spacing() const;
    void setSpacing( int spacing );

    const GeoSceneIcon* icon() const;
    GeoSceneIcon* icon();

 private:
    Q_DISABLE_COPY( GeoSceneItem )

    GeoSceneIcon* m_icon;

    QString m_name;
    QString m_text;
    QString m_connectTo;

    bool    m_checkable;
    int     m_spacing;
};

}

#endif
