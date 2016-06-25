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

#ifndef MARBLE_GEOSCENEPROPERTY_H
#define MARBLE_GEOSCENEPROPERTY_H

#include <QObject>
#include <QString>

#include <geodata_export.h>

#include "GeoDocument.h"

namespace Marble
{

/**
 * @short Settings property within a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneProperty : public QObject, 
                                        public GeoNode
{
    Q_OBJECT

 public:
    explicit GeoSceneProperty( const QString& name );
    virtual const char* nodeType() const;
    
    QString name() const;
    bool available() const;
    void setAvailable( bool available );
    bool defaultValue() const;
    void setDefaultValue( bool defaultValue );
    bool value() const;
    void setValue( bool value );

 Q_SIGNALS:
    void valueChanged( const QString&, bool );

 private:
    // FIXME: d-pointerfy

    QString  m_name;
    bool     m_available;
    bool     m_defaultValue;
    bool     m_value;
};

}

#endif
