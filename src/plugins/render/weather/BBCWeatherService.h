//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef BBCWEATHERSERVICE_H
#define BBCWEATHERSERVICE_H

#include "AbstractWeatherService.h"

#include "GeoDataLatLonAltBox.h"

namespace Marble
{
    
class BBCWeatherItem;
class StationListParser;

class BBCWeatherService : public AbstractWeatherService
{
    Q_OBJECT
 
 public:
    BBCWeatherService( QObject *parent );
    ~BBCWeatherService();
    
 public Q_SLOTS:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             MarbleDataFacade *facade,
                             qint32 number = 10 );
 
 private Q_SLOTS:
    void fetchStationList();

 private:
    void setupList();

    QList<BBCWeatherItem*> m_items;
    bool m_parsingStarted;
    StationListParser *m_parser;
    GeoDataLatLonAltBox m_scheduledBox;
    qint32 m_scheduledNumber;
    MarbleDataFacade *m_scheduledFacade;
};

} // namespace Marble

#endif // BBCWEATHERSERVICE_H
