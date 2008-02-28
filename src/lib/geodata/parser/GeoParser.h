/*
    Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>

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
    along with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef GeoParser_h
#define GeoParser_h

#include <QXmlStreamReader>

#include "GeoDocument.h"

typedef int GeoDataGenericSourceType;

class GeoParser : public QXmlStreamReader {
public:
    GeoParser(int sourceType);
    virtual ~GeoParser();

    // Main API.
    bool read(QIODevice*);

    // If parsing was successful, retreive the resulting document and set the contained m_document pointer to 0.
    GeoDocument* releaseDocument();
    GeoDocument* activeDocument() { return m_document; }

    // Used by tag handlers, to be overriden by GeoDataParser/GeoSceneParser
    virtual bool isValidElement(const QString& tagName) const;

protected:
    // To be implemented by GeoDataParser/GeoSceneParser
    virtual bool isValidDocumentElement() const = 0;
    virtual void raiseDocumentElementError();

    virtual GeoDocument* createDocument() const = 0;

private:
    void parseDocument();

protected:
    GeoDocument* m_document;
    GeoDataGenericSourceType m_source;
};

#endif // GeoParser_h
