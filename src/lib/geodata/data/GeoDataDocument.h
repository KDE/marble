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

#ifndef GEODATADOCUMENT_H
#define GEODATADOCUMENT_H


#include <QVector>
#include <QHash>

#include "GeoDataFolder.h"

class GeoDataStyle;

class GeoDataDocument : public GeoDataFolder {
public:
    GeoDataDocument();
    ~GeoDataDocument();

    // Read-only part of the API - to be used after _parsing_,
    // to actually process the data within Marble that we parsed.
    const QVector<GeoDataFolder>& folders() const;

    // Read-write part of the API - to be used within the tag handlers,
    // to fill our document with content.
    void addFolder(const GeoDataFolder&);
    void addStyle( GeoDataStyle* style );
    const GeoDataStyle& getStyle( QString styleId ) const;

  private:
    QVector<GeoDataFolder> m_folders;
    QHash < QString, GeoDataStyle* > m_styleHash;
};

#endif // GEODATADOCUMENT_H
