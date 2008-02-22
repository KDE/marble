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

#include "GeoSceneParser.h"
#include "GeoSceneTagHandler.h"

// DGML support
#include "DGMLElementDictionary.h"

using namespace GeoSceneElementDictionary;

GeoSceneParser::GeoSceneParser(GeoSceneDataSource source)
    : QXmlStreamReader()
    , m_source(source)
{
}

GeoSceneParser::~GeoSceneParser()
{
}

GeoSceneDocument& GeoSceneParser::document()
{
    return m_document;
}

const GeoSceneDocument& GeoSceneParser::document() const
{
    return m_document;
}

bool GeoSceneParser::read(QIODevice* device)
{
    // Start with a fresh document
    m_document = GeoSceneDocument();

    // Set data source
    setDevice(device);

    // Parse it baby!
    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            bool valid = false;

            switch (m_source) {
            case GeoSceneData_DGML:
                valid = isValidElement(dgmlTag_dgml);
                break;
            default:
                break;
            }

            if (valid) {
                parseDocument();
                break;
            } else {
                switch (m_source) {
                case GeoSceneData_DGML:
                    raiseError(QObject::tr("The file is not a valid DGML 2.0 file"));
                    break;                
                default:
                    raiseError(QObject::tr("File format unrecognized"));
                    break;
                }
            }
        }
    }

    if (error())
        qDebug() << "[GeoSceneParser::read] -> Error occured:" << errorString();

    return !error();
}

bool GeoSceneParser::isValidElement(const QString& tagName) const
{
    if (name() != tagName)
        return false;

    // FIXME: Now that we supported intermixed documents (ie. gpx in kml)
    // this check is not valid anymore. Just by knowing the document type
    // we can't say wheter the element is valid. We probably should check
    // wheter it's _either_ georss, or gpx or kml. To be discussed.

    switch (m_source) {
    case GeoSceneData_DGML:
        return (namespaceUri() == dgmlTag_nameSpace20);    
    default:
        break;
    }

    // Should never be reached.
    Q_ASSERT(false);
    return false;
}

void GeoSceneParser::parseDocument()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            const QString& tagName = name().toString();

            // Check if we have any registered handlers for this node
            if (const GeoSceneTagHandler* handler = GeoSceneTagHandler::recognizes(tagName))
                handler->parse(*this);

            parseDocument();
        }
    }
}
