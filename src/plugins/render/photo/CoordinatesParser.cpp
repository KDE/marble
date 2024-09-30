// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "CoordinatesParser.h"

// Marble
#include "GeoDataCoordinates.h"

using namespace Marble;

CoordinatesParser::CoordinatesParser(GeoDataCoordinates *coordinates)
    : m_coordinates(coordinates)
{
}

bool CoordinatesParser::read(QIODevice *device)
{
    setDevice(device);

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (name() == QLatin1StringView("rsp")) {
                if (attributes().value(QLatin1StringView("stat")) == QLatin1StringView("ok")) {
                    readRsp();
                } else {
                    raiseError(QObject::tr("Query failed"));
                }
            } else {
                raiseError(QObject::tr("The file is not a valid Flickr answer."));
            }
        }
    }

    return !error();
}

void CoordinatesParser::readUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            readUnknownElement();
    }
}

void CoordinatesParser::readRsp()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("photo"))
                readPhoto();
            else
                readUnknownElement();
        }
    }
}

void CoordinatesParser::readPhoto()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("photo"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("location"))
                readLocation();
            else
                readUnknownElement();
        }
    }
}

void CoordinatesParser::readLocation()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("location"));

    m_coordinates->setLatitude(attributes().value(QLatin1StringView("latitude")).toString().toDouble() * DEG2RAD);
    m_coordinates->setLongitude(attributes().value(QLatin1StringView("longitude")).toString().toDouble() * DEG2RAD);

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            break;
    }
}
