//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef ABSTRACTWEATHERSERVICE_H
#define ABSTRACTWEATHERSERVICE_H

// Marble
#include "WeatherModel.h"

namespace Marble
{


class AbstractWeatherService : public QObject
{
    Q_OBJECT
    
 public:
    explicit AbstractWeatherService( QObject *parent );
    virtual ~AbstractWeatherService();
    
 public Q_SLOTS:
    virtual void getAdditionalItems( const GeoDataLatLonAltBox& box,
                                     MarbleDataFacade *facade,
                                     qint32 number = 10 ) = 0;
    
 Q_SIGNALS:
    void requestedDownload( const QUrl& url, const QString& type, AbstractDataPluginItem *item );
    void createdItem( AbstractDataPluginItem *item );
};

} // namespace Marble

#endif // ABSTRACTWEATHERSERVICE_H
