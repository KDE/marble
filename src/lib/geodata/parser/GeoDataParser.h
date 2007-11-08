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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef GeoDataParser_h
#define GeoDataParser_h

#include <QHash>
#include <QXmlStreamReader>

#include "GeoDataDocument.h"

class GeoDataTagHandler;

enum GeoDataDataSource {
    GeoDataData_GeoRSS = 0,
    GeoDataData_GPX = 1,
    GeoDataData_KML = 2
};

class GeoDataParser : public QXmlStreamReader {
public:
    GeoDataParser(GeoDataDataSource source);
    virtual ~GeoDataParser();

    // Main API.
    bool read(QIODevice*);

    // Helper function for the tag handlers
    bool isValidElement(const QString& tagName) const;

    // If parsing was succesful, call this & be happy.
    GeoDataDocument& document();
    const GeoDataDocument& document() const;

private:
    void parseDocument();

private:
    GeoDataDocument m_document;
    GeoDataDataSource m_source;
};

#endif // GeoDataParser_h
