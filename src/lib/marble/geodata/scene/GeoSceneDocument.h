/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2007 Murad Tagirov <tmurad@gmail.com>

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

#ifndef MARBLE_GEOSCENEDOCUMENT_H
#define MARBLE_GEOSCENEDOCUMENT_H

#include <QObject>
#include "GeoDocument.h"

#include <geodata_export.h>

namespace Marble
{

class GeoSceneHead;
class GeoSceneLegend;
class GeoSceneMap;
class GeoSceneSettings;

class GeoSceneDocumentPrivate;

/**
 * @short A container for features parsed from the DGML file.
 */
class GEODATA_EXPORT GeoSceneDocument : public QObject, 
                                        public GeoDocument,
                                        public GeoNode
{
    Q_OBJECT

 public:
    GeoSceneDocument();
    ~GeoSceneDocument();
    
    virtual const char* nodeType() const;

    virtual bool isGeoSceneDocument() const { return true; }

    const GeoSceneHead* head() const;
    GeoSceneHead* head();

    const GeoSceneMap* map() const;
    GeoSceneMap* map();

    const GeoSceneSettings* settings() const;
    GeoSceneSettings* settings();

    const GeoSceneLegend* legend() const;
    GeoSceneLegend* legend();

 Q_SIGNALS:
    void valueChanged( const QString&, bool );

 private:
    Q_DISABLE_COPY( GeoSceneDocument )
    GeoSceneDocumentPrivate * const d;
};

}

#endif
