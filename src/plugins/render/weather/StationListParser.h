//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef STATIONLISTPARSER_H
#define STATIONLISTPARSER_H

// Qt
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QXmlStreamReader>

class QString;

namespace Marble
{

class BBCWeatherItem;
class GeoDataLatLonAltBox;
    
class StationListParser : public QXmlStreamReader
{
public:
    StationListParser( QObject *parent );

    QList<BBCWeatherItem *> read( QIODevice *device );

private:
    void readUnknownElement();
    void readStationList();
    void readStation();
    QString readCharacters();
    void readPoint( BBCWeatherItem *item );
    void readRegion( BBCWeatherItem *item );
    GeoDataLatLonAltBox readLatLonAltBox();

    QList<BBCWeatherItem *> m_list;
    QObject *m_parent;
};

} // Marble namespace

#endif // STATIONLISTPARSER_H
