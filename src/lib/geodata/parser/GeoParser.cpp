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
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QDebug>

#include "GeoParser.h"
#include "GeoTagHandler.h"

GeoParser::GeoParser(GeoDataGenericSourceType source)
    : QXmlStreamReader()
    , m_document(0)
    , m_source(source)
{
}

GeoParser::~GeoParser()
{
}

bool GeoParser::read(QIODevice* device)
{
    // Assert previous document got released.
    Q_ASSERT(!m_document);
    m_document = createDocument();
    Q_ASSERT(m_document);

    // Set data source
    setDevice(device);

    // Start parsing
    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (isValidDocumentElement()) {
                parseDocument();
                break;
            } else
                raiseDocumentElementError();
        }
    }

    if (error())
        qDebug() << "[GeoParser::read] -> Error occurred:" << errorString();

    return !error();
}

bool GeoParser::isValidElement(const QString& tagName) const
{
    return name() == tagName;
}

void GeoParser::parseDocument()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            const QString& tagName = name().toString();
            const QString& nameSpace = namespaceUri().toString();

            // Check if we have any registered handlers for this node
            if (const GeoTagHandler* handler = GeoTagHandler::recognizes(GeoTagHandler::QualifiedName(tagName, nameSpace)))
                handler->parse(*this);

            parseDocument();
        }
    }
}

void GeoParser::raiseDocumentElementError()
{
    raiseError(QObject::tr("File format unrecognized"));
}

GeoDocument* GeoParser::releaseDocument()
{
    GeoDocument* document = m_document;
    m_document = 0;
    return document;
}
