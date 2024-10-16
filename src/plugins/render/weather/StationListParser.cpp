// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "StationListParser.h"

// Marble
#include "BBCStation.h"
#include "GeoDataCoordinates.h"
#include "MarbleGlobal.h"

// Qt
#include <QFile>
#include <QString>

using namespace Marble;

StationListParser::StationListParser(QObject *parent)
    : QThread(parent)
    , QXmlStreamReader()
{
}

StationListParser::~StationListParser()
{
    wait(1000);
}

void StationListParser::read()
{
    m_list.clear();

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (name() == QLatin1StringView("StationList"))
                readStationList();
            else
                raiseError(QObject::tr("The file is not a valid file."));
        }
    }
}

QList<BBCStation> StationListParser::stationList() const
{
    return m_list;
}

void StationListParser::setPath(const QString &path)
{
    m_path = path;
}

void StationListParser::run()
{
    QFile file(m_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    setDevice(&file);
    read();
}

void StationListParser::readUnknownElement()
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

void StationListParser::readStationList()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("StationList"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("Station"))
                readStation();
            else
                readUnknownElement();
        }
    }
}

void StationListParser::readStation()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("Station"));

    BBCStation station;

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("name"))
                station.setName(readCharacters());
            else if (name() == QLatin1StringView("id"))
                station.setBbcId(readCharacters().toLong());
            else if (name() == QLatin1StringView("priority"))
                station.setPriority(readCharacters().toInt());
            else if (name() == QLatin1StringView("Point"))
                readPoint(&station);
            else
                readUnknownElement();
        }
    }

    // This find the right position in the sorted to insert the new item
    QList<BBCStation>::iterator i = std::lower_bound(m_list.begin(), m_list.end(), station);
    // Insert the item on the right position in the list
    m_list.insert(i, station);
}

QString StationListParser::readCharacters()
{
    Q_ASSERT(isStartElement());

    QString string;

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            readUnknownElement();
        }

        if (isCharacters()) {
            string = text().toString();
        }
    }

    return string;
}

void StationListParser::readPoint(BBCStation *station)
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("Point"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("coordinates")) {
                QString coorString = readCharacters();
                QStringList coorList = coorString.split(QLatin1Char(','));

                if (coorList.size() >= 2) {
                    GeoDataCoordinates coordinates(coorList.at(0).toFloat() * DEG2RAD, coorList.at(1).toFloat() * DEG2RAD);
                    station->setCoordinate(coordinates);
                }
            } else
                readUnknownElement();
        }
    }
}

#include "moc_StationListParser.cpp"
