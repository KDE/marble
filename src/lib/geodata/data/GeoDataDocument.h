/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef GeoDataDocument_h
#define GeoDataDocument_h

#include <QVector>

#include "GeoDataFolder.h"

class GeoDataDocument {
public:
    GeoDataDocument();
    ~GeoDataDocument();

    // Read-only part of the API - to be used after _parsing_,
    // to actually process the data within Marble that we parsed.
    const QVector<GeoDataFolder>& folders() const;

    // Read-write part of the API - to be used within the tag handlers,
    // to fill our document with content.
    void addFolder(const GeoDataFolder&);

private:
    QVector<GeoDataFolder> m_folders;
};

#endif // GeoDataDocument_h
