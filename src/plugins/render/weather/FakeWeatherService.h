//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef FAKEWEATHERSERVICE_H
#define FAKEWEATHERSERVICE_H

#include "AbstractWeatherService.h"

namespace Marble
{

class FakeWeatherService : public AbstractWeatherService
{
    Q_OBJECT
 
 public:
    explicit FakeWeatherService( const MarbleModel *model, QObject *parent );
    ~FakeWeatherService();
    
 public Q_SLOTS:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             qint32 number = 10 );
    virtual void getItem( const QString &id );
};

} // namespace Marble

#endif // FAKEWEATHERSERVICE_H
