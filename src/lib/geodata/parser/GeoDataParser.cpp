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

#include <QDebug>

#include "GeoDataParser.h"
#include "GeoDataTagHandler.h"

// TODO: GeoRSS support
// #include "GeoRSSElementDictionary.h"

// GPX support
#include "GPXElementDictionary.h"

// KML support
#include "KMLElementDictionary.h"

using namespace GeoDataElementDictionary;

GeoDataParser::GeoDataParser(GeoDataDataSource source)
    : QXmlStreamReader()
    , m_source(source)
{
}

GeoDataParser::~GeoDataParser()
{
}

GeoDataDocument* GeoDataParser::releaseDocument()
{
    GeoDataDocument* document = m_document;
    m_document = 0;
    return document;
}

GeoDataDocument& GeoDataParser::document() const
{
    return *m_document;
}

bool GeoDataParser::read(QIODevice* device)
{
    // Assert previous document got released.
    Q_ASSERT(!m_document);
    m_document = new GeoDataDocument;

    // Set data source
    setDevice(device);

    // Parse it baby!
    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            bool valid = false;

            switch (m_source) {
            // TODO: case GeoDataData_GeoRSS:
            case GeoDataData_GPX:
                valid = isValidElement(gpxTag_gpx);
                break;                
            case GeoDataData_KML:
                valid = isValidElement(kmlTag_kml);
                break;
            default:
                break;
            }

            if (valid) {
                parseDocument();
                break;
            } else {
                switch (m_source) {
                // TODO: case GeoDataData_GeoRSS:
                case GeoDataData_GPX:
                    raiseError(QObject::tr("The file is not a valid GPX 1.0 / 1.1 file"));
                    break;                
                case GeoDataData_KML:
                    raiseError(QObject::tr("The file is not a valid KML 2.0 / 2.1 file"));
                    break;
                default:
                    raiseError(QObject::tr("File format unrecognized"));
                    break;
                }
            }
        }
    }

    if (error())
        qDebug() << "[GeoDataParser::read] -> Error occurred:" << errorString();

    return !error();
}

bool GeoDataParser::isValidElement(const QString& tagName) const
{
    if (name() != tagName)
        return false;

    // FIXME: Now that we supported intermixed documents (ie. gpx in kml)
    // this check is not valid anymore. Just by knowing the document type
    // we can't say wheter the element is valid. We probably should check
    // wheter it's _either_ georss, or gpx or kml. To be discussed.

    switch (m_source) {
    // TODO: case GeoDataData_GeoRSS:
    case GeoDataData_GPX:
        return (namespaceUri() == gpxTag_nameSpace10 || namespaceUri() == gpxTag_nameSpace11);
    case GeoDataData_KML:
        return (namespaceUri() == kmlTag_nameSpace20 || namespaceUri() == kmlTag_nameSpace21);    
    default:
        break;
    }

    // Should never be reached.
    Q_ASSERT(false);
    return false;
}

void GeoDataParser::parseDocument()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            const QString& tagName = name().toString();

            // Check if we have any registered handlers for this node
            if (const GeoDataTagHandler* handler = GeoDataTagHandler::recognizes(tagName))
                handler->parse(*this);

            parseDocument();
        }
    }
}
