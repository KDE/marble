//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef FAKEWEATHERITEM_H
#define FAKEWEATHERITEM_H

#include "WeatherItem.h"

class QString;

namespace Marble
{

class FakeWeatherItem : public WeatherItem
{
 public:
    explicit FakeWeatherItem( QObject *parent = nullptr );
    ~FakeWeatherItem() override;
    
    QString service() const override;
    void addDownloadedFile( const QString& url, const QString& type ) override;
};

} // namespace Marble

#endif // FAKEWEATHERITEM_H
