//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
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
    ~FakeWeatherService() override;
    
 public Q_SLOTS:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             qint32 number = 10 ) override;
    void getItem( const QString &id ) override;
};

} // namespace Marble

#endif // FAKEWEATHERSERVICE_H
