//
// This file is part of the Marble Virtual Globe.
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

#include <QtCore/QStringList>

namespace Marble
{

class BBCItemGetter;
class BBCStation;
class GeoDataLatLonAltBox;
class StationListParser;

class BBCWeatherService : public AbstractWeatherService
{
    Q_OBJECT
 
 public:
    BBCWeatherService( QObject *parent );
    ~BBCWeatherService();

    void setFavoriteItems( const QStringList& favorite );
    void setFavoriteItemsOnly( bool favoriteOnly );
    
 public Q_SLOTS:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             const MarbleModel *model,
                             qint32 number = 10 );

 private Q_SLOTS:
    void fetchStationList();
    void createItem( BBCStation station );

 private:
    void setupList();
    QList<BBCStation> filterStationsList( const QStringList& favorites );

    QList<BBCStation> m_stationList;
    bool m_parsingStarted;
    StationListParser *m_parser;
    BBCItemGetter *m_itemGetter;
};

} // namespace Marble

#endif // BBCWEATHERSERVICE_H
